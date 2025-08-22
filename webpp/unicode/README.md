# Unicode

### Standards:

| Name                        | Standard                                                     | Usage | Version | Date       |
|-----------------------------|--------------------------------------------------------------|-------|---------|------------|
| Unicode Normalization Forms | [UTX #15](https://www.unicode.org/reports/tr15/tr15-54.html) |       | 15.1.0  | 2023-08-12 |
| IDNA                        | UTS #46                                                      |       |         |            |

**Please**: if there's a [new version](https://www.unicode.org/versions/latest/) of these documents, notify us or help
us update the codes to the latest versions.

More info about Unicode: W3C's [Character Model for the World Wide Web: String Matching](https://www.w3.org/TR/charmod-norm/)

## Unicode General Category

Each Unicode code point belongs to a category (look at Section [*4.5 General
Category* of the Unicode Standard](http://www.unicode.org/versions/Unicode15.0.0/ch04.pdf#G124142))

| Category | Subcategory | Description                         |
|----------|-------------|-------------------------------------|
| L        | Lu          | Letter, uppercase                   |
| L        | Ll          | Letter, lowercase                   |
| L        | Lt          | Letter, titlecase                   |
| L        | Lm          | Letter, modifier                    |
| L        | Lo          | Letter, other                       |
|          |             |                                     |
| M        | Mn          | Mark, non-spacing                   |
| M        | Mc          | Mark, spacing combining             |
| M        | Me          | Mark, enclosing                     |
|          |             |                                     |
| N        | Nd          | Number, decimal digit               |
| N        | Nl          | Number, letter                      |
| N        | No          | Number, other                       |
|          |             |                                     |
| P        | Pc          | Punctuation, connector              |
| P        | Pd          | Punctuation, dash                   |
| P        | Ps          | Punctuation, open                   |
| P        | Pe          | Punctuation, close                  |
| P        | Pi          | Punctuation, initial quote          |
| P        | Pf          | Punctuation, final quote            |
| P        | Po          | Punctuation, other                  |
|          |             |                                     |
| S        | Sm          | Symbol, math                        |
| S        | Sc          | Symbol, currency                    |
| S        | Sk          | Symbol, modifier                    |
| S        | So          | Symbol, other                       |
|          |             |                                     |
| Z        | Zs          | Separator, space                    |
| Z        | Zl          | Separator, line                     |
| Z        | Zp          | Separator, paragraph                |
|          |             |                                     |
| C        | Cc          | Other, control                      |
| C        | Cf          | Other, format                       |
| C        | Cs          | Other, surrogate                    |
| C        | Co          | Other, private use                  |
| C        | Cn          | Other, not assigned or noncharacter |

The Unicode standard defines the following General Categories:

* **Letter (L)**: Characters that are used for writing words and text, including uppercase and lowercase letters,
  titlecase letters, modifier letters, and other letters.
* **Mark (M)**: Characters that are used to modify or decorate other characters, such as diacritical marks, enclosing
  marks, and spacing combining marks.
* **Number (N)**: Characters that are used for numerical values, including decimal digits, letter numbers, and other
  numbers.
* **Punctuation (P)**: Characters that are used to separate or group text, such as commas, periods, and quotation marks.
* **Symbol (S)**: Characters that are used for various purposes, such as mathematical symbols, currency symbols, and
  other symbols.
* **Separator (Z)**: Characters that are used to separate text into lines or paragraphs, such as line separators,
  paragraph separators, and space separators.
* **Other (C)**: Characters that do not fit into any of the above categories, such as control characters, format
  characters, and private use characters.

### Decomposition

**Decomposition mapping**: A mapping from a character to a sequence of one or more
characters that is a canonical or compatibility equivalent.

- The default value for the `Decomposition_Mapping` property for a code point is the code point itself.

## IDNA: Internationalized Domain Names in Applications

Support for the Internationalized Domain Names in
Applications (IDNA) protocol as specified in [RFC 5891](https://tools.ietf.org/html/rfc5891). This is the latest version
of the protocol and is sometimes referred to as **IDNA 2008**.

This library also provides support for Unicode Technical
Standard 46, [Unicode IDNA Compatibility Processing](https://unicode.org/reports/tr46/).

### Standards that are implemented:

| Name                                  | Standard                                                     | Usage                        | Version | Last Updated |
|---------------------------------------|--------------------------------------------------------------|------------------------------|---------|--------------|
| Unicode IDNA Compatibility Processing | [UTS #46](https://www.unicode.org/reports/tr46/tr46-31.html) | [URL Standard](../README.md) | 15.1.0  | 2023-09-05   |

**Please**: if there's a [new version](https://www.unicode.org/versions/latest/) of these documents, notify us or help
us update the codes to the latest versions.

### Labels

[According to UTS #46](https://www.unicode.org/reports/tr46/#Notation), **A label is a substring of a domain name**.

That substring is bounded on both sides by either the start or the end of the string,
or any of the following characters, called label-separators:

- `U+002E ( . )`:  FULL STOP
- `U+FF0E ( ． )`: FULLWIDTH FULL STOP
- `U+3002 ( 。 )`: IDEOGRAPHIC FULL STOP
- `U+FF61 ( ｡ )`:  HALFWIDTH IDEOGRAPHIC FULL STOP

### A-Labels vs U-Labels

The A-label is the ASCII-compatible encoded (ACE) representation of an
internationalized domain name, used for internal transmission within the DNS protocol. It always commences with the
prefix "xn--" and is not intended for end-user display.

On the other hand, the U-label is the Unicode representation of
an internationalized domain name, which is the form shown to the end-user.
It contains one or more characters outside the Basic Latin range (U+0000 to U+007F) and
meets IDNA2008 requirements for labels with such characters.

When a domain name will later be presented to the user in its native form, conversion from the A-label to the U-label
should be performed.

### STD3 Rules

Refer to [UTS #46](https://www.unicode.org/reports/tr46/#STD3_Rules). We don't need to implement these rules.

### IDNA2008

The IDNA2008 specification is defined by a cluster of IETF RFCs:

- IDNA: Definitions and Document Framework
  https://www.rfc-editor.org/info/rfc5890
- IDNA Protocol
  https://www.rfc-editor.org/info/rfc5891
- IDNA for Applications
  https://www.rfc-editor.org/info/rfc5892
- Right-to-Left Scripts for IDNA
  https://www.rfc-editor.org/info/rfc5893

There is also an informative document:

- IDNA: Background, Explanation, and Rationale
  https://www.rfc-editor.org/info/rfc5894
