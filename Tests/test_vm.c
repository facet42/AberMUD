/*
 * Tests for the action/condition VM: CondCode.c's Cnd_* evaluators and
 * ActionCode.c's Act_* executors, driven through the real dispatch path
 * (ExecTable/RunLine in TableDriver.c) rather than called directly, so
 * the bytecode decoding (ArgNum/ArgItem/ArgWord, flag indirection,
 * $1/$2/$ME/$AC/$RM item sentinels) is exercised too, not just the
 * individual functions. See vm_stubs.c for what's faked out and why.
 *
 * IMPORTANT: RunLine() (TableDriver.c) always `return(0);` -- a failed
 * condition just `break`s out of that line's remaining opcodes, it does
 * not change the return value, and ExecTable() passes that same 0
 * straight back. So a table line's condition outcome is only observable
 * as a side effect (did the actions after it run or not), never via the
 * return code. Every condition test below appends a marker action
 * (LET MARK 1) after the condition and checks whether *that* ran.
 */
#include "../Source/System.h"
#include "../Source/User.h"
#include "test_framework.h"

extern ITEM *Item1, *Item2;
extern int Noun1, Adj1, Noun2, Adj2, Prep;

/* ArgItem()'s sentinel values (TableDriver.c:194-213): a pair-packed
 * pointer that decodes to exactly 1/3/5/7/9 is substituted for $1/$2/$ME/
 * $AC/$RM instead of being used as a literal address. Two ushorts each,
 * high word first (PairArg: (*x)<<16 | x[1]) -- since these values are
 * tiny, the high word is always 0. */
#define ARG1	0, 1
#define ARG2	0, 3

#define OP(name) (unsigned short)FindCnd(name)

/* Scratch flag slot used only as the "did the condition pass" marker;
 * none of these tests use it for anything else. */
#define MARK 500

/* Builds "<condition opcode(s)...> LET MARK 1 <end>" -- CodeSucceeds()
 * below reports whether the marker action actually ran. */
#define IF_THEN_MARK(...) { __VA_ARGS__, OP("LET"), MARK, 1, CMD_EOL }

static int RunCode(unsigned short *code)
{
	LINE line = { -1, -1, -1, code, NULL };
	TABLE table = { 0, &line, NULL, NULL };
	return ExecTable(&table);
}

static int CodeSucceeds(unsigned short *code)
{
	SetFlag(MARK, 0);
	RunCode(code);
	return GetFlag(MARK) == 1;
}

static void reset_item(ITEM *i)
{
	memset(i, 0, sizeof(ITEM));
	i->it_Adjective = -1;
	i->it_Noun = -1;
}

static void attach_player(ITEM *item, PLAYER *p, short level, short flags)
{
	memset(p, 0, sizeof(PLAYER));
	p->pl_Sub.pr_Key = KEY_PLAYER;
	p->pl_Sub.pr_Next = item->it_Properties;
	p->pl_Level = level;
	p->pl_Flags = flags;
	item->it_Properties = (SUB *)p;
}

static void attach_object(ITEM *item, OBJECT *o, short flags)
{
	memset(o, 0, sizeof(OBJECT));
	o->ob_Sub.pr_Key = KEY_OBJECT;
	o->ob_Sub.pr_Next = item->it_Properties;
	o->ob_Flags = flags;
	item->it_Properties = (SUB *)o;
}

static void attach_room(ITEM *item, ROOM *r, unsigned short flags)
{
	memset(r, 0, sizeof(ROOM));
	r->rm_Sub.pr_Key = KEY_ROOM;
	r->rm_Sub.pr_Next = item->it_Properties;
	r->rm_Flags = flags;
	item->it_Properties = (SUB *)r;
}

/* ---- item-relationship conditions ---- */

static void test_cnd_at_and_notat(void)
{
	ITEM room, elsewhere, player;
	reset_item(&room); reset_item(&elsewhere); reset_item(&player);
	player.it_Parent = &room;
	SetMe(&player);
	Item1 = &room;

	unsigned short at_room[] = IF_THEN_MARK(OP("AT"), ARG1);
	TEST_CHECK(CodeSucceeds(at_room));

	Item1 = &elsewhere;
	TEST_CHECK(!CodeSucceeds(at_room));

	unsigned short notat[] = IF_THEN_MARK(OP("NOTAT"), ARG1);
	TEST_CHECK(CodeSucceeds(notat));	/* player not at elsewhere */
	Item1 = &room;
	TEST_CHECK(!CodeSucceeds(notat));	/* player is at room */
}

static void test_cnd_present_and_absent(void)
{
	ITEM room, player, held_item, room_item, faraway_item;
	reset_item(&room); reset_item(&player);
	reset_item(&held_item); reset_item(&room_item); reset_item(&faraway_item);
	player.it_Parent = &room;
	SetMe(&player);
	held_item.it_Parent = &player;		/* carried */
	room_item.it_Parent = &room;		/* in the same room */
	/* faraway_item stays parentless */

	unsigned short present[] = IF_THEN_MARK(OP("PRESENT"), ARG1);
	Item1 = &held_item;
	TEST_CHECK(CodeSucceeds(present));
	Item1 = &room_item;
	TEST_CHECK(CodeSucceeds(present));
	Item1 = &faraway_item;
	TEST_CHECK(!CodeSucceeds(present));

	unsigned short absent[] = IF_THEN_MARK(OP("ABSENT"), ARG1);
	Item1 = &faraway_item;
	TEST_CHECK(CodeSucceeds(absent));
	Item1 = &held_item;
	TEST_CHECK(!CodeSucceeds(absent));
}

static void test_cnd_carried_and_notcarr(void)
{
	ITEM player, other, held, dropped;
	reset_item(&player); reset_item(&other); reset_item(&held); reset_item(&dropped);
	SetMe(&player);
	held.it_Parent = &player;
	dropped.it_Parent = &other;

	unsigned short carried[] = IF_THEN_MARK(OP("CARRIED"), ARG1);
	Item1 = &held;
	TEST_CHECK(CodeSucceeds(carried));
	Item1 = &dropped;
	TEST_CHECK(!CodeSucceeds(carried));

	unsigned short notcarr[] = IF_THEN_MARK(OP("NOTCARR"), ARG1);
	Item1 = &dropped;
	TEST_CHECK(CodeSucceeds(notcarr));
	Item1 = &held;
	TEST_CHECK(!CodeSucceeds(notcarr));
}

static void test_cnd_isat_isby(void)
{
	ITEM room, other_room, a, b, c;
	reset_item(&room); reset_item(&other_room);
	reset_item(&a); reset_item(&b); reset_item(&c);
	a.it_Parent = &room;
	b.it_Parent = &room;
	c.it_Parent = &other_room;

	/* ISAT $1 $2 means O_PARENT($1)==$2: `a` is parented in `room` */
	unsigned short isat[] = IF_THEN_MARK(OP("ISAT"), ARG1, ARG2);
	Item1 = &a; Item2 = &room;
	TEST_CHECK(CodeSucceeds(isat));
	Item2 = &other_room;
	TEST_CHECK(!CodeSucceeds(isat));

	unsigned short isnotat[] = IF_THEN_MARK(OP("ISNOTAT"), ARG1, ARG2);
	Item1 = &a; Item2 = &other_room;
	TEST_CHECK(CodeSucceeds(isnotat));
	Item2 = &room;
	TEST_CHECK(!CodeSucceeds(isnotat));

	unsigned short isby[] = IF_THEN_MARK(OP("ISBY"), ARG1, ARG2);
	Item1 = &a; Item2 = &b;	/* both parented in `room` */
	TEST_CHECK(CodeSucceeds(isby));
	Item2 = &c;			/* parented in `other_room` */
	TEST_CHECK(!CodeSucceeds(isby));

	unsigned short isnotby[] = IF_THEN_MARK(OP("ISNOTBY"), ARG1, ARG2);
	Item1 = &a; Item2 = &c;
	TEST_CHECK(CodeSucceeds(isnotby));
	Item2 = &b;
	TEST_CHECK(!CodeSucceeds(isnotby));
}

static void test_cnd_worn(void)
{
	ITEM player, coat, rock;
	OBJECT coat_obj, rock_obj;
	reset_item(&player); reset_item(&coat); reset_item(&rock);
	SetMe(&player);
	attach_object(&coat, &coat_obj, OB_WORN);
	attach_object(&rock, &rock_obj, 0);
	coat.it_Parent = &player;
	rock.it_Parent = &player;

	unsigned short worn[] = IF_THEN_MARK(OP("WORN"), ARG1);
	Item1 = &coat;
	TEST_CHECK(CodeSucceeds(worn));
	Item1 = &rock;
	TEST_CHECK(!CodeSucceeds(worn));	/* carried but not worn */

	unsigned short notworn[] = IF_THEN_MARK(OP("NOTWORN"), ARG1);
	Item1 = &rock;
	TEST_CHECK(CodeSucceeds(notworn));
	Item1 = &coat;
	TEST_CHECK(!CodeSucceeds(notworn));
}

static void test_cnd_isin_isnotin(void)
{
	ITEM box, room, marble, pebble;
	reset_item(&box); reset_item(&room); reset_item(&marble); reset_item(&pebble);
	box.it_Parent = &room;
	marble.it_Parent = &box;	/* nested two levels deep */
	pebble.it_Parent = &room;	/* sibling of box, not inside it */

	unsigned short isin[] = IF_THEN_MARK(OP("ISIN"), ARG1, ARG2);
	Item1 = &box; Item2 = &marble;
	TEST_CHECK(CodeSucceeds(isin));	/* box contains marble */
	Item1 = &box; Item2 = &pebble;
	TEST_CHECK(!CodeSucceeds(isin));

	unsigned short isnotin[] = IF_THEN_MARK(OP("ISNOTIN"), ARG1, ARG2);
	Item1 = &box; Item2 = &pebble;
	TEST_CHECK(CodeSucceeds(isnotin));
	Item1 = &box; Item2 = &marble;
	TEST_CHECK(!CodeSucceeds(isnotin));
}

/* ---- flag arithmetic conditions ---- */

static void test_cnd_zero_notzero(void)
{
	WipeFlags();
	unsigned short zero[] = IF_THEN_MARK(OP("ZERO"), 0);
	TEST_CHECK(CodeSucceeds(zero));
	SetFlag(0, 7);
	TEST_CHECK(!CodeSucceeds(zero));

	unsigned short notzero[] = IF_THEN_MARK(OP("NOTZERO"), 0);
	TEST_CHECK(CodeSucceeds(notzero));
	SetFlag(0, 0);
	TEST_CHECK(!CodeSucceeds(notzero));
}

static void test_cnd_eq_noteq_gt_lt(void)
{
	WipeFlags();
	SetFlag(10, 5);

	unsigned short eq[] = IF_THEN_MARK(OP("EQ"), 10, 5);
	TEST_CHECK(CodeSucceeds(eq));
	unsigned short eq_false[] = IF_THEN_MARK(OP("EQ"), 10, 6);
	TEST_CHECK(!CodeSucceeds(eq_false));

	unsigned short noteq[] = IF_THEN_MARK(OP("NOTEQ"), 10, 6);
	TEST_CHECK(CodeSucceeds(noteq));

	unsigned short gt[] = IF_THEN_MARK(OP("GT"), 10, 4);
	TEST_CHECK(CodeSucceeds(gt));
	unsigned short gt_false[] = IF_THEN_MARK(OP("GT"), 10, 5);
	TEST_CHECK(!CodeSucceeds(gt_false));

	unsigned short lt[] = IF_THEN_MARK(OP("LT"), 10, 6);
	TEST_CHECK(CodeSucceeds(lt));
	unsigned short lt_false[] = IF_THEN_MARK(OP("LT"), 10, 5);
	TEST_CHECK(!CodeSucceeds(lt_false));
}

static void test_cnd_flag_vs_flag(void)
{
	WipeFlags();
	SetFlag(1, 5);
	SetFlag(2, 5);
	SetFlag(3, 9);

	unsigned short eqf[] = IF_THEN_MARK(OP("EQF"), 1, 2);
	TEST_CHECK(CodeSucceeds(eqf));
	unsigned short eqf_false[] = IF_THEN_MARK(OP("EQF"), 1, 3);
	TEST_CHECK(!CodeSucceeds(eqf_false));

	unsigned short nef[] = IF_THEN_MARK(OP("NOTEQF"), 1, 3);
	TEST_CHECK(CodeSucceeds(nef));

	unsigned short ltf[] = IF_THEN_MARK(OP("LTF"), 1, 3);
	TEST_CHECK(CodeSucceeds(ltf));

	unsigned short gtf[] = IF_THEN_MARK(OP("GTF"), 3, 1);
	TEST_CHECK(CodeSucceeds(gtf));
}

/* ---- parsed-word conditions ---- */

static void test_cnd_word_codes(void)
{
	Adj1 = 4; Noun1 = 7; Adj2 = 2; Noun2 = 9; Prep = 3;

	unsigned short adj1[] = IF_THEN_MARK(OP("ADJ1"), 4);
	TEST_CHECK(CodeSucceeds(adj1));
	unsigned short adj1_false[] = IF_THEN_MARK(OP("ADJ1"), 5);
	TEST_CHECK(!CodeSucceeds(adj1_false));

	unsigned short noun1[] = IF_THEN_MARK(OP("NOUN1"), 7);
	TEST_CHECK(CodeSucceeds(noun1));

	unsigned short adj2[] = IF_THEN_MARK(OP("ADJ2"), 2);
	TEST_CHECK(CodeSucceeds(adj2));

	unsigned short noun2[] = IF_THEN_MARK(OP("NOUN2"), 9);
	TEST_CHECK(CodeSucceeds(noun2));

	unsigned short prep[] = IF_THEN_MARK(OP("PREP"), 3);
	TEST_CHECK(CodeSucceeds(prep));
	unsigned short prep_false[] = IF_THEN_MARK(OP("PREP"), 4);
	TEST_CHECK(!CodeSucceeds(prep_false));
}

/* ---- probability boundary ---- */

static void test_cnd_chance_boundaries(void)
{
	/* RandPerc() (TableDriver.c) returns a value in [0,99]; CHANCE n is
	 * RandPerc()<n, so n=0 can never succeed and n=100 always does --
	 * this is deterministic regardless of the RNG's actual draw. */
	unsigned short never[] = IF_THEN_MARK(OP("CHANCE"), 0);
	TEST_CHECK(!CodeSucceeds(never));

	unsigned short always[] = IF_THEN_MARK(OP("CHANCE"), 100);
	TEST_CHECK(CodeSucceeds(always));
}

/* ---- class/kind and bit-flag conditions ---- */

static void test_cnd_isplayer_isroom_isobject(void)
{
	ITEM p_item, r_item, o_item, plain;
	PLAYER p; ROOM r; OBJECT o;
	reset_item(&p_item); reset_item(&r_item); reset_item(&o_item); reset_item(&plain);
	attach_player(&p_item, &p, 1, 0);
	attach_room(&r_item, &r, 0);
	attach_object(&o_item, &o, 0);

	unsigned short isplayer[] = IF_THEN_MARK(OP("ISPLAYER"), ARG1);
	Item1 = &p_item; TEST_CHECK(CodeSucceeds(isplayer));
	Item1 = &r_item; TEST_CHECK(!CodeSucceeds(isplayer));

	unsigned short isroom[] = IF_THEN_MARK(OP("ISROOM"), ARG1);
	Item1 = &r_item; TEST_CHECK(CodeSucceeds(isroom));
	Item1 = &plain; TEST_CHECK(!CodeSucceeds(isroom));

	unsigned short isobject[] = IF_THEN_MARK(OP("ISOBJECT"), ARG1);
	Item1 = &o_item; TEST_CHECK(CodeSucceeds(isobject));
	Item1 = &plain; TEST_CHECK(!CodeSucceeds(isobject));
}

static void test_cnd_state(void)
{
	ITEM door;
	reset_item(&door);
	door.it_State = 2;

	unsigned short state[] = IF_THEN_MARK(OP("STATE"), ARG1, 2);
	Item1 = &door;
	TEST_CHECK(CodeSucceeds(state));
	unsigned short state_false[] = IF_THEN_MARK(OP("STATE"), ARG1, 3);
	TEST_CHECK(!CodeSucceeds(state_false));
}

static void test_cnd_pflag_oflag_rflag(void)
{
	ITEM p_item, o_item, r_item;
	PLAYER p; OBJECT o; ROOM r;
	reset_item(&p_item); reset_item(&o_item); reset_item(&r_item);
	attach_player(&p_item, &p, 1, PL_BLIND);
	attach_object(&o_item, &o, OB_WORN);
	attach_room(&r_item, &r, RM_DARK);

	/* PL_BLIND is bit value 8 = 1<<3 */
	unsigned short pflag[] = IF_THEN_MARK(OP("PFLAG"), ARG1, 3);
	Item1 = &p_item;
	TEST_CHECK(CodeSucceeds(pflag));
	unsigned short pflag_false[] = IF_THEN_MARK(OP("PFLAG"), ARG1, 4);
	TEST_CHECK(!CodeSucceeds(pflag_false));

	/* OB_WORN is bit value 4 = 1<<2 */
	unsigned short oflag[] = IF_THEN_MARK(OP("OFLAG"), ARG1, 2);
	Item1 = &o_item;
	TEST_CHECK(CodeSucceeds(oflag));

	/* RM_DARK is bit value 1 = 1<<0 */
	unsigned short rflag[] = IF_THEN_MARK(OP("RFLAG"), ARG1, 0);
	Item1 = &r_item;
	TEST_CHECK(CodeSucceeds(rflag));
}

static void test_cnd_pflag_on_non_player_is_false(void)
{
	/* Cnd_PFlag/OFlag/RFlag all guard PlayerOf()/ObjectOf()/RoomOf()
	 * returning NULL by still consuming the ArgNum() bit-index (so the
	 * bytecode stream stays in sync) and reporting false. */
	ITEM plain;
	reset_item(&plain);
	unsigned short pflag[] = IF_THEN_MARK(OP("PFLAG"), ARG1, 0);
	Item1 = &plain;
	TEST_CHECK(!CodeSucceeds(pflag));
}

static void test_cnd_level(void)
{
	ITEM p_item;
	PLAYER p;
	reset_item(&p_item);
	attach_player(&p_item, &p, 5, 0);
	SetMe(&p_item);

	unsigned short level[] = IF_THEN_MARK(OP("LEVEL"), 5);
	TEST_CHECK(CodeSucceeds(level));	/* level >= 5 */
	unsigned short level_false[] = IF_THEN_MARK(OP("LEVEL"), 6);
	TEST_CHECK(!CodeSucceeds(level_false));
}

static void test_cnd_ifdeaf_ifblind(void)
{
	ITEM p_item;
	PLAYER p;
	reset_item(&p_item);
	attach_player(&p_item, &p, 1, PL_DEAF);
	SetMe(&p_item);

	unsigned short ifdeaf[] = IF_THEN_MARK(OP("IFDEAF"));
	TEST_CHECK(CodeSucceeds(ifdeaf));
	unsigned short ifblind[] = IF_THEN_MARK(OP("IFBLIND"));
	TEST_CHECK(!CodeSucceeds(ifblind));
}

static void test_cnd_cansee(void)
{
	ITEM p_item, visible, hidden;
	PLAYER p;
	reset_item(&p_item); reset_item(&visible); reset_item(&hidden);
	attach_player(&p_item, &p, 5, 0);
	SetMe(&p_item);
	visible.it_Perception = 5;
	hidden.it_Perception = 10;

	unsigned short cansee[] = IF_THEN_MARK(OP("CANSEE"), ARG1);
	Item1 = &visible;
	TEST_CHECK(CodeSucceeds(cansee));
	Item1 = &hidden;
	TEST_CHECK(!CodeSucceeds(cansee));
}

static void test_cnd_is(void)
{
	ITEM a, b;
	reset_item(&a); reset_item(&b);
	unsigned short is_same[] = IF_THEN_MARK(OP("IS"), ARG1, ARG2);
	Item1 = &a; Item2 = &a;
	TEST_CHECK(CodeSucceeds(is_same));
	Item2 = &b;
	TEST_CHECK(!CodeSucceeds(is_same));
}

/* ---- flag-arithmetic actions ---- */

static void test_act_let_add_sub(void)
{
	WipeFlags();
	unsigned short let[] = { OP("LET"), 0, 10, CMD_EOL };
	RunCode(let);
	TEST_CHECK_EQ_INT(GetFlag(0), 10);

	unsigned short add[] = { OP("ADD"), 0, 5, CMD_EOL };
	RunCode(add);
	TEST_CHECK_EQ_INT(GetFlag(0), 15);

	unsigned short sub[] = { OP("SUB"), 0, 4, CMD_EOL };
	RunCode(sub);
	TEST_CHECK_EQ_INT(GetFlag(0), 11);
}

static void test_act_addf_subf(void)
{
	WipeFlags();
	SetFlag(1, 100);
	SetFlag(2, 30);

	unsigned short addf[] = { OP("ADDF"), 1, 2, CMD_EOL };
	RunCode(addf);
	TEST_CHECK_EQ_INT(GetFlag(1), 130);

	unsigned short subf[] = { OP("SUBF"), 1, 2, CMD_EOL };
	RunCode(subf);
	TEST_CHECK_EQ_INT(GetFlag(1), 100);
}

static void test_act_mul_div_mod(void)
{
	WipeFlags();
	SetFlag(0, 7);

	unsigned short mul[] = { OP("MUL"), 0, 6, CMD_EOL };
	RunCode(mul);
	TEST_CHECK_EQ_INT(GetFlag(0), 42);

	unsigned short div[] = { OP("DIV"), 0, 6, CMD_EOL };
	RunCode(div);
	TEST_CHECK_EQ_INT(GetFlag(0), 7);

	unsigned short mod[] = { OP("MOD"), 0, 4, CMD_EOL };
	RunCode(mod);
	TEST_CHECK_EQ_INT(GetFlag(0), 3);
}

static void test_act_div_mod_by_zero_leaves_flag_unchanged(void)
{
	/* Act_Div/Act_Mod guard against division by zero and return without
	 * touching the flag (SysSupport.c-style guarded actions elsewhere
	 * in this codebase do the same). */
	WipeFlags();
	SetFlag(0, 9);
	unsigned short div0[] = { OP("DIV"), 0, 0, CMD_EOL };
	RunCode(div0);
	TEST_CHECK_EQ_INT(GetFlag(0), 9);

	unsigned short mod0[] = { OP("MOD"), 0, 0, CMD_EOL };
	RunCode(mod0);
	TEST_CHECK_EQ_INT(GetFlag(0), 9);
}

static void test_act_mulf_divf(void)
{
	WipeFlags();
	SetFlag(0, 6);
	SetFlag(1, 7);

	unsigned short mulf[] = { OP("MULF"), 0, 1, CMD_EOL };
	RunCode(mulf);
	TEST_CHECK_EQ_INT(GetFlag(0), 42);

	SetFlag(1, 6);
	unsigned short divf[] = { OP("DIVF"), 0, 1, CMD_EOL };
	RunCode(divf);
	TEST_CHECK_EQ_INT(GetFlag(0), 7);
}

static void test_act_set_clear(void)
{
	WipeFlags();
	unsigned short set[] = { OP("SET"), 0, CMD_EOL };
	RunCode(set);
	TEST_CHECK_EQ_INT(GetFlag(0), 255);

	unsigned short clear[] = { OP("CLEAR"), 0, CMD_EOL };
	RunCode(clear);
	TEST_CHECK_EQ_INT(GetFlag(0), 0);
}

static void test_act_inc_dec(void)
{
	/* Act_Inc/Act_Dec (ActionCode.c) cap it_State to the 0-3 range used
	 * by OBJECT's 4-state text slots: INC is a no-op once state is
	 * already >2, DEC once it's already <1. */
	ITEM dummy;
	reset_item(&dummy);
	dummy.it_State = 1;

	unsigned short inc[] = { OP("INC"), ARG1, CMD_EOL };
	Item1 = &dummy;
	RunCode(inc);
	TEST_CHECK_EQ_INT(dummy.it_State, 2);

	unsigned short dec[] = { OP("DEC"), ARG1, CMD_EOL };
	RunCode(dec);
	TEST_CHECK_EQ_INT(dummy.it_State, 1);

	dummy.it_State = 3;
	RunCode(inc);
	TEST_CHECK_EQ_INT(dummy.it_State, 3);	/* already at the cap */

	dummy.it_State = 0;
	RunCode(dec);
	TEST_CHECK_EQ_INT(dummy.it_State, 0);	/* already at the floor */
}

static void test_act_pset_pclear(void)
{
	ITEM p_item;
	PLAYER p;
	reset_item(&p_item);
	attach_player(&p_item, &p, 1, 0);
	Item1 = &p_item;

	/* bit 3 = PL_BLIND(8) */
	unsigned short pset[] = { OP("PSET"), ARG1, 3, CMD_EOL };
	RunCode(pset);
	TEST_CHECK((p.pl_Flags & PL_BLIND) != 0);

	unsigned short pclear[] = { OP("PCLEAR"), ARG1, 3, CMD_EOL };
	RunCode(pclear);
	TEST_CHECK((p.pl_Flags & PL_BLIND) == 0);
}

static void test_act_oset_oclear(void)
{
	ITEM o_item;
	OBJECT o;
	reset_item(&o_item);
	attach_object(&o_item, &o, 0);
	Item1 = &o_item;

	/* bit 2 = OB_WORN(4) */
	unsigned short oset[] = { OP("OSET"), ARG1, 2, CMD_EOL };
	RunCode(oset);
	TEST_CHECK((o.ob_Flags & OB_WORN) != 0);

	unsigned short oclear[] = { OP("OCLEAR"), ARG1, 2, CMD_EOL };
	RunCode(oclear);
	TEST_CHECK((o.ob_Flags & OB_WORN) == 0);
}

static void test_act_rset_rclear(void)
{
	ITEM r_item;
	ROOM r;
	reset_item(&r_item);
	attach_room(&r_item, &r, 0);
	Item1 = &r_item;

	/* bit 0 = RM_DARK(1) */
	unsigned short rset[] = { OP("RSET"), ARG1, 0, CMD_EOL };
	RunCode(rset);
	TEST_CHECK((r.rm_Flags & RM_DARK) != 0);

	unsigned short rclear[] = { OP("RCLEAR"), ARG1, 0, CMD_EOL };
	RunCode(rclear);
	TEST_CHECK((r.rm_Flags & RM_DARK) == 0);
}

static void test_act_setstate(void)
{
	ITEM item;
	reset_item(&item);
	Item1 = &item;
	unsigned short setstate[] = { OP("SETSTATE"), ARG1, 3, CMD_EOL };
	RunCode(setstate);
	TEST_CHECK_EQ_INT(item.it_State, 3);
}

/* ---- a full "IF ... THEN ..." line, conditions gating actions ---- */

static void test_line_stops_at_failed_condition(void)
{
	ITEM room, elsewhere, player;
	reset_item(&room); reset_item(&elsewhere); reset_item(&player);
	player.it_Parent = &elsewhere;
	SetMe(&player);
	Item1 = &room;
	WipeFlags();

	/* IF AT $1 THEN LET 0 99 -- player is NOT at $1, so the LET must
	 * never run (observable only via the flag; RunLine's return code
	 * doesn't carry this, see the file-level comment above). */
	unsigned short code[] = {
		OP("AT"), ARG1,
		OP("LET"), 0, 99,
		CMD_EOL
	};
	RunCode(code);
	TEST_CHECK_EQ_INT(GetFlag(0), 0);

	player.it_Parent = &room;
	RunCode(code);
	TEST_CHECK_EQ_INT(GetFlag(0), 99);
}

int main(int argc, char *argv[])
{
	(void)argc; (void)argv;
	RUN_TEST(test_cnd_at_and_notat);
	RUN_TEST(test_cnd_present_and_absent);
	RUN_TEST(test_cnd_carried_and_notcarr);
	RUN_TEST(test_cnd_isat_isby);
	RUN_TEST(test_cnd_worn);
	RUN_TEST(test_cnd_isin_isnotin);
	RUN_TEST(test_cnd_zero_notzero);
	RUN_TEST(test_cnd_eq_noteq_gt_lt);
	RUN_TEST(test_cnd_flag_vs_flag);
	RUN_TEST(test_cnd_word_codes);
	RUN_TEST(test_cnd_chance_boundaries);
	RUN_TEST(test_cnd_isplayer_isroom_isobject);
	RUN_TEST(test_cnd_state);
	RUN_TEST(test_cnd_pflag_oflag_rflag);
	RUN_TEST(test_cnd_pflag_on_non_player_is_false);
	RUN_TEST(test_cnd_level);
	RUN_TEST(test_cnd_ifdeaf_ifblind);
	RUN_TEST(test_cnd_cansee);
	RUN_TEST(test_cnd_is);
	RUN_TEST(test_act_let_add_sub);
	RUN_TEST(test_act_addf_subf);
	RUN_TEST(test_act_mul_div_mod);
	RUN_TEST(test_act_div_mod_by_zero_leaves_flag_unchanged);
	RUN_TEST(test_act_mulf_divf);
	RUN_TEST(test_act_set_clear);
	RUN_TEST(test_act_inc_dec);
	RUN_TEST(test_act_pset_pclear);
	RUN_TEST(test_act_oset_oclear);
	RUN_TEST(test_act_rset_rclear);
	RUN_TEST(test_act_setstate);
	RUN_TEST(test_line_stops_at_failed_condition);
	return test_summary_and_exit();
}
