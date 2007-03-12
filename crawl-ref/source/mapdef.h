/*
 * mapdef.h:
 * Header for map structures used by the level compiler.
 *
 * NOTE: When we refer to map, this could be a full map, filling an entire
 * level or a minivault that occupies just a portion of the level.
 */

#ifndef __MAPDEF_H__
#define __MAPDEF_H__

#include <string>
#include <vector>

#include "enum.h"
#include "externs.h"

enum map_flags
{
    MAPF_PANDEMONIUM_VAULT = 0x01,     // A pandemonium minivault.

    MAPF_MIRROR_VERTICAL   = 0x10,     // The map may be mirrored vertically
    MAPF_MIRROR_HORIZONTAL = 0x20,     // may be mirrored horizontally.
    MAPF_ROTATE            = 0x40      // may be rotated
};

class level_range
{
public:
    int shallowest, deepest;

public:
    level_range(int s = -1, int d = -1);

    void set(int s, int d = -1);
    void reset();
    bool contains(int depth) const;

    bool valid() const;
    int span() const;
};

typedef std::pair<int,int> glyph_weighted_replacement_t;
typedef std::vector<glyph_weighted_replacement_t> glyph_replacements_t;

class subst_spec
{
public:
    subst_spec(int torepl, bool fix, const glyph_replacements_t &repls);

    int key() const
    {
        return (foo);
    }
    
    int value();

private:
    int foo;        // The thing to replace.
    bool fix;       // If true, the first replacement fixes the value.
    int frozen_value;
    
    glyph_replacements_t repl;
};

class map_lines
{
public:
    map_lines();

    void add_line(const std::string &s);
    std::string add_subst(const std::string &st);
    std::string add_shuffle(const std::string &s);

    void set_orientation(const std::string &s);

    int width() const;
    int height() const;

    int glyph(int x, int y) const;
    bool is_solid(int gly) const;
    
    bool solid_borders(map_section_type border);
    
    void subst();
    void resolve_shuffles();

    // Make all lines the same length.
    void normalise(char fillc = 'x');

    // Rotate 90 degrees either clockwise or anticlockwise
    void rotate(bool clockwise);
    void hmirror();
    void vmirror();

    void clear();

    const std::vector<std::string> &get_lines() const;

private:
    void resolve_shuffle(const std::string &shuffle);
    void subst(std::string &s, subst_spec &spec);
    void check_borders();
    std::string shuffle(std::string s);
    std::string block_shuffle(const std::string &s);
    std::string check_shuffle(std::string &s);
    std::string check_block_shuffle(const std::string &s);
    std::string clean_shuffle(std::string s);
    std::string parse_glyph_replacements(std::string s,
                                         glyph_replacements_t &gly);

private:
    std::vector<subst_spec>  substitutions;
    std::vector<std::string> shuffles;
    std::vector<std::string> lines;
    int map_width;
    bool solid_north, solid_east, solid_south, solid_west;
    bool solid_checked;
};

struct mons_spec
{
    int mid;
    int genweight;
    bool fix_mons;
    bool generate_awake;

    mons_spec(int id = RANDOM_MONSTER, int gw = 10, bool _fixmons = false,
              bool awaken = false)
        : mid(id), genweight(gw), fix_mons(_fixmons), generate_awake(awaken)
    {
    }
};

class mons_list
{
public:
    mons_list();

    void clear();

    mons_spec get_monster(int index);

    // Returns an error string if the monster is unrecognised.
    std::string add_mons(const std::string &s, bool fix_slot = false);

    size_t size() const { return mons.size(); }

private:
    typedef std::vector<mons_spec> mons_spec_list;

    struct mons_spec_slot
    {
        mons_spec_list mlist;
        bool fix_slot;

        mons_spec_slot(const mons_spec_list &list, bool fix = false)
            : mlist(list), fix_slot(fix)
        {
        }

        mons_spec_slot()
            : mlist(), fix_slot(false)
        {
        }
    };

private:
    int mons_by_name(std::string name) const;
    mons_spec_slot parse_mons_spec(std::string spec);
    mons_spec pick_monster(mons_spec_slot &slot);
    int fix_demon(int id) const;

private:
    std::vector< mons_spec_slot > mons;
    std::string error;
};

struct item_spec
{
    int genweight;
    
    int base_type, sub_type;
    int allow_uniques;
    int level;
    int race;

    item_spec() : genweight(10), base_type(OBJ_RANDOM), sub_type(OBJ_RANDOM),
        allow_uniques(1), level(-1), race(MAKE_ITEM_RANDOM_RACE)
    {
    }
};
typedef std::vector<item_spec> item_spec_list;

class item_list
{
public:
    item_list() : items() { }

    void clear();

    item_spec get_item(int index);
    size_t size() const { return items.size(); }

    std::string add_item(const std::string &spec, bool fix = false);

private:
    struct item_spec_slot
    {
        item_spec_list ilist;
        bool fix_slot;

        item_spec_slot() : ilist(), fix_slot(false)
        {
        }
    };
    
private:
    item_spec item_by_specifier(const std::string &spec);
    item_spec_slot parse_item_spec(std::string spec);
    item_spec parse_single_spec(std::string s);
    void parse_raw_name(std::string name, item_spec &spec);
    void parse_random_by_class(std::string c, item_spec &spec);
    item_spec pick_item(item_spec_slot &slot);

private:
    std::vector<item_spec_slot> items;
    std::string error;
};

struct feature_spec
{
    int genweight;
    int feat;
    int shop;
    int trap;
    int glyph;

    feature_spec(int f, int wt = 10)
        : genweight(wt), feat(f), shop(-1),
          trap(-1), glyph(-1)
    { }
    feature_spec() : genweight(0), feat(0), shop(-1), trap(-1), glyph(-1) { }
};

typedef std::vector<feature_spec> feature_spec_list;
struct feature_slot
{
    feature_spec_list feats;
    bool fix_slot;

    feature_slot();
    feature_spec get_feat();
};

struct keyed_mapspec
{
public:
    feature_slot feat;
    item_list    item;
    mons_list    mons;

public:
    keyed_mapspec();

    std::string set_feat(const std::string &s, bool fix);
    std::string set_mons(const std::string &s, bool fix);
    std::string set_item(const std::string &s, bool fix);

    feature_spec get_feat();
    mons_spec get_mons();
    item_spec get_item();

private:
    std::string err;

private:
    void parse_features(const std::string &);
    feature_spec_list parse_feature(const std::string &s);
    feature_spec parse_shop(std::string s, int weight);
    feature_spec parse_trap(std::string s, int weight);
};

typedef std::map<int, keyed_mapspec> keyed_specs;

// Not providing a constructor to make life easy for C-style initialisation.
class map_def
{
public:
    std::string     name;
    std::string     tags;
    std::string     place;
    level_range     depth;
    map_section_type orient;
    int             chance;
    long            flags;

    map_lines       map;
    mons_list       mons;
    item_list       items;

    keyed_specs     keyspecs;

public:
    void init();
    void hmirror();
    void vmirror();
    void rotate(bool clockwise);
    void normalise();
    void resolve();
    void fixup();

    keyed_mapspec *mapspec_for_key(int key);
    
    std::string add_key_item(const std::string &s);
    std::string add_key_mons(const std::string &s);
    std::string add_key_feat(const std::string &s);
    
    bool can_dock(map_section_type) const;
    coord_def dock_pos(map_section_type) const;
    coord_def float_dock();
    coord_def float_place();
    coord_def float_random_place() const;

    bool is_minivault() const;
    bool has_tag(const std::string &tag) const;

private:
    std::string add_key_field(
        const std::string &s,
        std::string (keyed_mapspec::*set_field)(
            const std::string &s, bool fixed));
};

class monster_chance
{
public:
    int mclass;
    int level;
    int rarity;
};

class level_def
{
public:
    // The range of levels to which this def applies.
    level_range range;

    // Can be empty, in which case the default colours are applied.
    std::string floor_colour, rock_colour;
        std::string tags;

    // The probability of requesting a random vault.
    int p_vault;

    // The probability of requesting a random minivault.
    int p_minivault;

    // If non-empty, any upstair will go straight to this level.
    std::string upstair_targ, downstair_targ;

    std::vector<monster_chance> monsters;
};

class dungeon_def
{
public:
    std::string idstr;
    int id;
    std::string short_desc, full_desc;

    std::vector<level_def> level_specs;

public:
    const level_def &specs(int subdepth);
};

std::string escape_string(std::string in, const std::string &toesc,
                          const std::string &escapewith);

#endif
