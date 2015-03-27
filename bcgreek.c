#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/** Bit masks for modifiers.  Maybe they should all be defined as macros or as
 * constants; in both cases the readability of the declaration suffers.
 */

#define msk_smooth  1
#define msk_rough  2
#define msk_acute  4
#define msk_grave  8
#define msk_circumflex  16
#define msk_iota  32
#define msk_diaeresis 64
#define msk_macron 128
#define msk_breve 256
#define msk_capital 512

static const int msk_no_breathings = ~(msk_smooth | msk_rough);
static const int msk_no_accents = ~(msk_acute | msk_grave | msk_circumflex);
static const int msk_no_lengths = ~(msk_macron | msk_breve);

static const int breathings = msk_smooth;
static const int accents = msk_acute;
static const int lengths = msk_macron;

/** smart_sigma is a global parameter which specifies if word final sigmas
 * should be automatically converted to the final form.
 */

char smart_sigma = 0;

/** The variants of a letter with possible diacritics are stored in arrays.
 * The actual form of the arrays is chosen to facilitate the search.
 */


char *alpha_variants[] = {
    "α", "ἀ", "ἁ",
    "ά", "ἄ", "ἅ",
    "ὰ", "ἂ", "ἃ",
    NULL, NULL, NULL,
    "ᾶ", "ἆ", "ἇ",
    "ᾳ", "ᾀ", "ᾁ",
    "ᾴ", "ᾄ", "ᾅ",
    "ᾲ", "ᾂ", "ᾃ",
    NULL, NULL, NULL,
    "ᾷ", "ᾆ", "ᾇ",
    "ᾱ", "ᾰ",
    "Α", "Ἀ", "Ἁ",
    "Ά", "Ἄ", "Ἅ",
    "Ὰ", "Ἂ", "Ἃ",
    NULL, NULL, NULL,
    "ᾶ", "Ἆ", "Ἇ",
    "ᾼ", "ᾈ", "ᾉ",
    "ᾴ", "ᾌ", "ᾍ",
    "ᾲ", "ᾊ", "ᾋ",
    NULL, NULL, NULL,
    "ᾷ", "ᾎ", "ᾏ",
    "Ᾱ", "Ᾰ"
};

char *eta_variants[] = {
    "η", "ἠ", "ἡ",
    "ή", "ἤ", "ἥ",
    "ὴ", "ἢ", "ἣ",
    NULL, NULL, NULL,
    "ῆ", "ἦ", "ἧ",
    "ῃ", "ᾐ", "ᾑ",
    "ῄ", "ᾔ", "ᾕ",
    "ῂ", "ᾒ", "ᾓ",
    NULL, NULL, NULL,
    "ῇ", "ᾖ", "ᾗ",
    "Η", "Ἠ", "Ἡ",
    "Ή", "Ἤ", "Ἥ",
    "Ὴ", "Ἢ", "Ἣ",
    NULL, NULL, NULL,
    "ῆ", "Ἦ", "Ἧ",
    "ῌ", "ᾘ", "ᾙ",
    "ῄ", "ᾜ", "ᾝ",
    "ῂ", "ᾚ", "ᾛ",
    NULL, NULL, NULL,
    "ῇ", "ᾞ", "ᾟ"
};

char *omega_variants[] = {
    "ω", "ὠ", "ὡ",
    "ώ", "ὤ", "ὥ",
    "ὼ", "ὢ", "ὣ",
    NULL, NULL, NULL,
    "ῶ", "ὦ", "ὧ",
    "ῳ", "ᾠ", "ᾡ",
    "ῴ", "ᾤ", "ᾥ",
    "ῲ", "ᾢ", "ᾣ",
    NULL, NULL, NULL,
    "ῷ", "ᾦ", "ᾧ"
    "Ω", "Ὠ", "Ὡ",
    "Ώ", "Ὤ", "Ὥ",
    "Ὼ", "Ὢ", "Ὣ",
    NULL, NULL, NULL,
    "ῶ", "Ὦ", "Ὧ",
    "ῼ", "ᾨ", "ᾩ",
    "ῴ", "ᾬ", "ᾭ",
    "ῲ", "ᾪ", "ᾫ",
    NULL, NULL, NULL,
    "ῷ", "ᾮ", "ᾯ"
};

char *iota_variants[] = {
    "ι", "ί", "ὶ", NULL, "ῖ",
    "ἰ", "ἴ", "ἲ", NULL, "ἶ",
    "ἱ", "ἵ", "ἳ", NULL, "ἷ",
    "ϊ", "ΐ", "ῒ", NULL, "ῗ",
    "ῑ", "ῐ",
    "Ι", "Ί", "Ὶ", NULL, "ῖ",
    "Ἰ", "Ἴ", "Ἲ", NULL, "Ἶ",
    "Ἱ", "Ἵ", "Ἳ", NULL, "Ἷ",
    "Ϊ", "ῒ", "ΐ", NULL, "ῗ",
    "Ῑ", "Ῐ"
};

char *upsilon_variants[] = {
    "υ", "ύ", "ὺ", NULL, "ῦ",
    "ὐ", "ὔ", "ὒ", NULL, "ὖ",
    "ὑ", "ὕ", "ὓ", NULL, "ὗ",
    "ϋ", "ΰ", "ῢ", NULL, "ῧ",
    "ῡ", "ῠ",
    "Υ", "Ύ", "Ὺ", NULL, "ῦ",
    NULL, NULL, NULL, NULL, NULL,
    "Ὑ", "Ὕ", "Ὓ", NULL, "Ὗ",
    "Ϋ", "ΰ", "ῢ", NULL, "ῧ",
    "Ῡ", "Ῠ"
};

char *epsilon_variants[] = {
    "ε", "ἐ", "ἑ",
    "έ", "ἔ", "ἕ",
    "ὲ", "ἒ", "ἓ",
    "Ε", "Ἐ", "Ἑ",
    "Έ", "Ἔ", "Ἕ",
    "Ὲ", "Ἒ", "Ἓ"
};

char *omicron_variants[] = {
    "ο", "ὀ", "ὁ",
    "ό", "ὄ", "ὅ",
    "ὸ", "ὂ", "ὃ",
    "Ο", "Ὀ", "Ὁ",
    "Ό", "Ὄ", "Ὅ",
    "Ὸ", "Ὂ", "Ὃ"
};


/* Lookup functions */

char* alpha_variant(int mods);
char* hw_variant(int mods, char *vars[]);
char* iy_variant(int mods, char *vars[]);
char* eo_variant(int mods, char *vars[]);

/* Auxiliary dispatch-related functions */

int mod2bit(int c, int *mask);
int read_mods(int mask, FILE *stream, int *c);
int dispatch_a(FILE *in, FILE *out);
int dispatch_e(FILE *in, FILE *out);
int dispatch_o(FILE *in, FILE *out);
int dispatch_i(FILE *in, FILE *out);
int dispatch_u(FILE *in, FILE *out);
int dispatch_h(FILE *in, FILE *out);
int dispatch_w(FILE *in, FILE *out);
int dispatch_r(FILE *in, FILE *out);
int dispatch_s(FILE *in, FILE *out);
char* capital_variant(char c, int mods);
int dispatch_capital(FILE *in, FILE *out);


/** The program consequently reads characters from one stream, processes them,
 * and writes the output to another stream.  dispatch_char is the core
 * function.  It takes a character as an argument and is supposed to write
 * something to a given output stream.  However, it may also need to read a few
 * more characters to determine what to output.  So the function takes three
 * arguments.
 *
 * dispatch_char returns a char to be processed next.
 *
 * If the given character uniquely defines a Greek letter, that letter is
 * immediately written to the output stream, and the function returns the next
 * character read from the input stream (which can occasionally be EOF).  The
 * cases of letters which can take accents and of the `*' character, which
 * potentially gives rise to a capital letter, are dealt with individually.
 *
 */

#define CASE_FPUTS_GETC(x, y) case x: fputs(y, out); return getc(in);

int dispatch_char(char c, FILE *in, FILE *out)
{
    switch(c) {
        CASE_FPUTS_GETC('b', "β")
        CASE_FPUTS_GETC('c', "ξ")
        CASE_FPUTS_GETC('d', "δ")
        CASE_FPUTS_GETC('f', "φ")
        CASE_FPUTS_GETC('g', "γ")
        CASE_FPUTS_GETC('j', "ς")
        CASE_FPUTS_GETC('k', "κ")
        CASE_FPUTS_GETC('l', "λ")
        CASE_FPUTS_GETC('m', "μ")
        CASE_FPUTS_GETC('n', "ν")
        CASE_FPUTS_GETC('p', "π")
        CASE_FPUTS_GETC('q', "θ")
        CASE_FPUTS_GETC('t', "τ")
        CASE_FPUTS_GETC('v', "ϝ")
        CASE_FPUTS_GETC('x', "χ")
        CASE_FPUTS_GETC('y', "ψ")
        CASE_FPUTS_GETC('z', "ζ")
        CASE_FPUTS_GETC('B', "β")
        CASE_FPUTS_GETC('C', "ξ")
        CASE_FPUTS_GETC('D', "δ")
        CASE_FPUTS_GETC('F', "φ")
        CASE_FPUTS_GETC('G', "γ")
        CASE_FPUTS_GETC('J', "ς")
        CASE_FPUTS_GETC('K', "κ")
        CASE_FPUTS_GETC('L', "λ")
        CASE_FPUTS_GETC('M', "μ")
        CASE_FPUTS_GETC('N', "ν")
        CASE_FPUTS_GETC('P', "π")
        CASE_FPUTS_GETC('Q', "θ")
        CASE_FPUTS_GETC('T', "τ")
        CASE_FPUTS_GETC('V', "ϝ")
        CASE_FPUTS_GETC('X', "χ")
        CASE_FPUTS_GETC('Y', "ψ")
        CASE_FPUTS_GETC('Z', "ζ")
        CASE_FPUTS_GETC('\'', "'")
        CASE_FPUTS_GETC(':', "·")
        case 'a':
        case 'A':
            return dispatch_a(in, out);
        case 'e':
        case 'E':
            return dispatch_e(in, out);
        case 'h':
        case 'H':
            return dispatch_h(in, out);
        case 'i':
        case 'I':
            return dispatch_i(in, out);
        case 'o':
        case 'O':
            return dispatch_o (in, out);
        case 'r':
        case 'R':
            return dispatch_r (in, out);
        case 's':
        case 'S':
            return dispatch_s (in, out);
        case 'u':
        case 'U':
            return dispatch_u(in, out);
        case 'w':
        case 'W':
            return dispatch_w(in, out);
        case '*':
            return dispatch_capital(in, out);
        default:
            putc(c, out);
            return getc(in);
    }
}

/** The function `convert' processes the input stream in a loop by constanly
 * evoking `dispatch_char' and feeding its output to the next invocation.
 */

void convert(FILE *in, FILE *out)
{
    int c = getc(in);
    while (c != EOF) {
        c = dispatch_char(c, in, out);
    }

}


/* Bit mask:
 * (smooth rough) (acute grave circumflex) iota diaeresis (macron breve)
 * the ones in parentheses are incompatible
 * macron and breve are incompatible with others
 * smooth and rough are incompatible with diaeresis
 * iota and diaeresis are incompatible
 * Mask bits:
 * Bit 0: inadmissible
 * Bit 1: admissible
 */


/** mod2bit returns the bit representation of a beta code modifier.  It also
 * takes a mask of admissible modifiers and *modifies it*.
 */

// EOF -> 0, all right
int mod2bit(int c, int *mask)
{
    int res;
    switch (c) {
        case ')':
            res = *mask & msk_smooth;
            *mask &= msk_no_breathings & msk_no_lengths;
            return res;
        case '(':
            res = *mask & msk_rough;
            *mask &= msk_no_breathings & msk_no_lengths;
            return res;
        case '/':
            res = *mask & msk_acute;
            *mask &= msk_no_accents & msk_no_lengths;
            return res;
        case '\\':
            res = *mask & msk_grave;
            *mask &= msk_no_accents & msk_no_lengths;
            return res;
        case '=':
            res = *mask & msk_circumflex;
            *mask &= msk_no_accents & msk_no_lengths;
            return res;
        case '|':
            res = *mask & msk_iota;
            *mask &= (~msk_iota) & msk_no_lengths & (~msk_diaeresis);
            return res;
        case '+':
            res = *mask & msk_diaeresis;
            *mask &= (~msk_iota) & msk_no_lengths & (~msk_diaeresis);
            return res;
        case '&':
            res = *mask & msk_macron;
            *mask = 0;
            return res;
        case '\'':
            res = *mask & msk_breve;
            *mask = 0;
            return res;
        default:
            return 0;
    }
}

/** `read_mods' reads modifiers in a loop ORing their bit values.  It
 * accepts a mask and a pointer to an integer, which will point to the first
 * character after the modifiers.
 *
 * Implementation note: as a side effect, mod2bit modifies the mask, but here
 * it's only a local copy.
 *
 */

int read_mods(int mask, FILE *stream, int *c)
{
    int mods = 0;
    int mod = 0;
    while ((mod = mod2bit(*c = getc(stream), &mask))) {
        mods |= mod;
    }
    return mods;
}

/** When dispatching a letter with possible diacritics, generally we read the
 * diacritics from the input stream and output the correspondent unicode
 * character.  The diacritics is read by the function read_mods.  It also
 * accepts a mask for possible modifiers of a given letter and a pointer to an
 * integer, which is the first character after all the modifiers.
 *
 * The case of rho is treated separately as it's very simple.
 *
 * The case of sigma is also treated differently.  It doesn't accept modifiers,
 * but can be changed to the final sigma.
 */


int dispatch_a(FILE *in, FILE *out)
{
    int c;
    fputs(alpha_variant(read_mods(~msk_diaeresis, in, &c)), out);
    return c;
}

int dispatch_e(FILE *in, FILE *out)
{
    int c;
    fputs(eo_variant(read_mods((~msk_iota)
                    & (~msk_diaeresis)
                    & msk_no_lengths,
                    in, &c), epsilon_variants), out);
    return c;
}

int dispatch_o(FILE *in, FILE *out)
{
    int c;
    fputs(eo_variant(read_mods((~msk_iota)
                    & (~msk_diaeresis)
                    & msk_no_lengths,
                    in, &c), omicron_variants), out);
    return c;
}

int dispatch_i(FILE *in, FILE *out)
{
    int c;
    fputs(iy_variant(read_mods(~msk_iota, in, &c), iota_variants), out);
    return c;
}

int dispatch_u(FILE *in, FILE *out)
{
    int c;
    fputs(iy_variant(read_mods(~msk_iota, in, &c), upsilon_variants), out);
    return c;
}

int dispatch_h(FILE *in, FILE *out)
{
    int c;
    fputs(hw_variant(read_mods((~msk_diaeresis) & msk_no_lengths, in, &c),
                eta_variants), out);
    return c;
}

int dispatch_w(FILE *in, FILE *out)
{
    int c;
    fputs(hw_variant(read_mods((~msk_diaeresis) & msk_no_lengths, in, &c),
                omega_variants), out);
    return c;
}

int dispatch_r(FILE *in, FILE *out)
{
    int c = getc(in);
    switch (c) {
        case '(':
            fputs("ῥ", out);
            return getc(in);
        case ')':
            fputs("ῤ", out);
            return getc(in);
        default:
            fputs("ρ", out);
            return c;
    }
}

int dispatch_s(FILE *in, FILE *out)
{
    if (smart_sigma) {
        int c = getc(in);
        if (((('a' <= c) && (c <= 'z')))
                || (('A' <= c) && (c <= 'Z'))) {
            fputs("σ", out);
        } else {
            fputs("ς", out);
        }
        return c;
    } else {
        fputs("σ", out);
        return getc(in);
    }
}

/** `capital_variant': given a beta code character and its modifiers in the bit
 * form, return the Greek letter.  In case there are accents, the function
 * checks if they are valid for the given letter.  This function is partly
 * analogous to `dispatch_char' and `dispatch_...'.
 */


#define CASE_RETURN(x, y) case x: return y;

char* capital_variant(char c, int mods)
{
    if (mods == msk_capital) {
        switch(c) {
            CASE_RETURN('a', "Α")
            CASE_RETURN('b', "Β")
            CASE_RETURN('c', "Ξ")
            CASE_RETURN('d', "Δ")
            CASE_RETURN('e', "Ε")
            CASE_RETURN('f', "Φ")
            CASE_RETURN('g', "Γ")
            CASE_RETURN('h', "Η")
            CASE_RETURN('i', "Ι")
            CASE_RETURN('k', "Κ")
            CASE_RETURN('l', "Λ")
            CASE_RETURN('m', "Μ")
            CASE_RETURN('n', "Ν")
            CASE_RETURN('o', "Ο")
            CASE_RETURN('p', "Π")
            CASE_RETURN('q', "Θ")
            CASE_RETURN('r', "Ρ")
            CASE_RETURN('s', "Σ")
            CASE_RETURN('t', "Τ")
            CASE_RETURN('u', "Υ")
            CASE_RETURN('v', "Ϝ")
            CASE_RETURN('w', "Ω")
            CASE_RETURN('x', "Χ")
            CASE_RETURN('y', "Ψ")
            CASE_RETURN('z', "Ζ")
            CASE_RETURN('A', "Α")
            CASE_RETURN('B', "Β")
            CASE_RETURN('C', "Ξ")
            CASE_RETURN('D', "Δ")
            CASE_RETURN('E', "Ε")
            CASE_RETURN('F', "Φ")
            CASE_RETURN('G', "Γ")
            CASE_RETURN('H', "Η")
            CASE_RETURN('I', "Ι")
            CASE_RETURN('K', "Κ")
            CASE_RETURN('L', "Λ")
            CASE_RETURN('M', "Μ")
            CASE_RETURN('N', "Ν")
            CASE_RETURN('O', "Ο")
            CASE_RETURN('P', "Π")
            CASE_RETURN('Q', "Θ")
            CASE_RETURN('R', "Ρ")
            CASE_RETURN('S', "Σ")
            CASE_RETURN('T', "Τ")
            CASE_RETURN('U', "Υ")
            CASE_RETURN('V', "Ϝ")
            CASE_RETURN('W', "Ω")
            CASE_RETURN('X', "Χ")
            CASE_RETURN('Y', "Ψ")
            CASE_RETURN('Z', "Ζ")

            default: return NULL;
        }
    } else {
        switch (c) {
            case 'a':
            case 'A':
                if (((!(mods & (msk_diaeresis | msk_macron | msk_breve)))
                            && (mods != (msk_capital | msk_circumflex))
                            && (mods != (msk_capital | msk_iota | msk_acute))
                            && (mods != (msk_capital | msk_iota | msk_grave))
                            && (mods != (msk_capital | msk_iota | msk_circumflex)))
                        || (mods == (msk_capital | msk_macron))
                        || (mods == (msk_capital | msk_breve))) {
                    return alpha_variant(mods);
                } else {
                    return NULL;
                }
            case 'h':
            case 'H':
        if ((!(mods & (msk_diaeresis | msk_macron | msk_breve)))
                && (mods != (msk_capital | msk_circumflex))
                && (mods != (msk_capital | msk_iota | msk_acute))
                && (mods != (msk_capital | msk_iota | msk_grave))
                && (mods != (msk_capital | msk_iota | msk_circumflex))) {
            return hw_variant(mods, eta_variants);
        } else {
            return NULL;
        }
            case 'w':
            case 'W':
        if ((! (mods & (msk_diaeresis | msk_macron | msk_breve)))
                && (mods != (msk_capital | msk_circumflex))
                && (mods != (msk_capital | msk_iota | msk_acute))
                && (mods != (msk_capital | msk_iota | msk_grave))
                && (mods != (msk_capital | msk_iota | msk_circumflex))) {
            return hw_variant(mods, omega_variants);
        } else {
            return NULL;
        }
            case 'i':
            case 'I':
                if ((! (mods & (msk_diaeresis | msk_macron | msk_breve)))
                        || (mods == (msk_capital | msk_diaeresis))
                        || (mods == (msk_capital | msk_macron))
                        || (mods == (msk_capital | msk_breve))) {
                    return iy_variant(mods, iota_variants);
                } else {
                    return NULL;
                }
            case 'u':
            case 'U':
                if ((mods & msk_rough)
                        || (mods == (msk_capital | msk_acute))
                        || (mods == (msk_capital | msk_grave))
                        || (mods == (msk_capital | msk_diaeresis))
                        || (mods == (msk_capital | msk_macron))
                        || (mods == (msk_capital | msk_breve))) {
                    return iy_variant(mods, upsilon_variants);
                } else {
                    return NULL;
                }
            case 'e':
            case 'E':
                if ( !(mods & (msk_iota | msk_diaeresis
                                | msk_macron | msk_breve | msk_circumflex))) {
                    return eo_variant(mods, epsilon_variants);
                } else {
                    return NULL;
                }
            case 'o':
            case 'O':
                if ( !(mods & (msk_iota | msk_diaeresis
                                | msk_macron | msk_breve | msk_circumflex))) {
                    return eo_variant(mods, omega_variants);
                } else {
                    return NULL;
                }
            case 'r':
            case 'R':
                if ((mods == (msk_capital | msk_rough))) {
                    return "Ῥ";
                } else {
                    return NULL;
                }
            default:
                return NULL;
        }
    }
}

/** The dispatch of a capital letter is analogous to the procedure for small
 * letters.  We read modifiers following the asterisk until we get an
 * alphabetic character (or we don't).  The body of the function
 * `dispatch_capital' resembles the function `read_mods'; after the modifiers
 * are read, the function `capital_variant' returns the actual unicode
 * character.  The `capital_variant' backend takes care of only accepting valid
 * diacritics.
 */


/** Implementation
 *
 * dispatch_capital uses a mask in the same way as read_mods.  It also uses a
 * buffer to store the asterisk and the modifiers in case the characters can't
 * be converted into a valid Greek letter.
 */

int dispatch_capital(FILE *in, FILE *out)
{
    char buf[5];
    buf[0] = '*';

    int i = 0;

    int mask = ~0;

    int c;

    int mods = msk_capital;
    int mod;

    char* s;

    // loop: read a char
    // assign it to c and to buf[++i]
    // calculate the bit form of the modifier
    // if it's zero, break
    while ((mod = mod2bit( (buf[++i] = (c = getc(in))), &mask))) {
        mods |= mod;
    }
    if ((s = capital_variant(c, mods))) {
            fputs(s, out);
            return getc(in);
        } else {
            buf[i] = '\0';
            fputs(buf, out);
            return c;
        }

}

/** The lookup functions are unsafe and only perform the lookup.  The validity
 * of modifiers is checked elsewhere.
 */

char* alpha_variant(int mods)
{

    return alpha_variants[
        mods / breathings % 4 // breathing
        + mods / accents % 8 * 3 // + 3*accent
        + mods / msk_iota % 2 * 15 // + 15*iota
        + mods / msk_macron % 2 * 30 // + 30*macron
        + mods / msk_breve % 2 * 31 // + 31*breve
        + mods / msk_capital % 2 * 32 // + 32*capital
        ];
}

char* hw_variant(int mods, char *vars[])
{

    return vars[
        mods / breathings % 4 // breathing
        + mods / accents % 8 * 3 // + 3*accent
        + mods / msk_iota % 2 * 15 // + 15*iota
        + mods / msk_capital % 2 * 30 // + 30*capital
        ];
}

char* iy_variant(int mods, char *vars[])
{
    return vars[
        mods / accents % 8 // accent
        + (mods / breathings % 4 + mods / msk_diaeresis %2 * 3) * 5 // + 6* breathing, diaeresis = breathing #3
        + mods / msk_macron % 2 * 20 // + 20*macron
        + mods / msk_breve % 2 * 21 // + 21*breve
        + mods / msk_capital % 2 * 22 // + 22*capital
        ];
}

char* eo_variant(int mods, char *vars[])
{
    return vars[ mods / breathings % 4 // breathing
        + (mods / accents % 4) * 3 // + 3*accent
        + mods / msk_capital % 2 * 9 // + 9*capital
        ];
}


/*
 *                      User interface
 */

void usage(FILE *out)
{
    fprintf(out, "usage: bcgreek [-s] [-f input_file] [-x string] [-o output_file]\n");
    fprintf(out, "Convert beta code into polytonic Greek.\n");
    fprintf(out, "  -s                    automatically convert S into final sigma\n");
    fprintf(out, "  -f input_file         input file; if this option is missing, standard input\n");
    fprintf(out, "                          is used\n");
    fprintf(out, "  -x string             process string\n");
    fprintf(out, "  -o output_file        output file; if this option is missing, standard\n");
    fprintf(out, "                          output is used\n");
    fprintf(out, "  -h                    display this help and exit\n");
}

int main(int argc, char *argv[])
{
    FILE *in;
    FILE *out;
    int oc;

    int sflag = 0;
    int fflag = 0;
    int oflag = 0;
    int xflag = 0;
    char *fvalue;
    char *ovalue;
    char *xvalue;

    while ((oc = getopt(argc, argv, "sf:o:hx:")) != -1) {
        switch (oc) {
            case 's':
                sflag = 1;
                break;
            case 'f':
                fflag = 1;
                fvalue = optarg;
                break;
            case 'o':
                oflag = 1;
                ovalue = optarg;
                break;
            case 'x':
                xflag = 1;
                xvalue = optarg;
                break;
            case 'h':
                usage(stdout);
                exit(0);
            default:
                usage(stderr);
                exit(1);
            }
    }

    if (optind < argc) {
        usage(stderr);
        exit(1);
    }

    if (sflag) smart_sigma = 1;

    // Input from stdin, from a file, or from a string.
    
    in = stdin;

    // Before any stream is open, make sure fflag and xflag are not set
    // simultaneously.
    if (fflag) {
        if (xflag) {
            fprintf(stderr, "%s: You can't use the -x and -f options simultaneously.\n", argv[0]);
            exit(1);
        }
        in = fopen(fvalue, "r");
        if (!in) {
            fprintf(stderr, "Cannot read from file %s.\n", fvalue);
            exit(1);
        }
    }

    if (xflag) in = fmemopen(xvalue, strlen(xvalue), "r");

    if (oflag) {
        out = fopen(ovalue, "w");
        if (!out) {
            fprintf(stderr, "Cannot write to file %s.\n", ovalue);
            exit(1);
        }
    } else {
        out = stdout;
    }

    convert(in, out);

    if (xflag) putc('\n', out);

    fclose(in);
    fclose(out);

    return 0;
}
