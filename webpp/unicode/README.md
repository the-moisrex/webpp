# Unicode

### Standards:

| Name                        | Standard                                                     | Usage                                                               | Version | Date       |
|-----------------------------|--------------------------------------------------------------|---------------------------------------------------------------------|---------|------------|
| Unicode Normalization Forms | [UTX #15](https://www.unicode.org/reports/tr15/tr15-54.html) | [IDNA](../uri/idna/README.md) (which is used in URL implementation) | 15.1.0  | 2023-08-12 |
| IDNA                        | UTS #46                                                      | Refer to [IDNA](../uri/idna/README.md)                              |         |            |

**Please**: if there's a [new version](https://www.unicode.org/versions/latest/) of these documents, notify us or help
us update the codes to the latest versions.

More info about unicode: W3C's [Character Model for the World Wide Web: String Matching](https://www.w3.org/TR/charmod-norm/)

## Unicode General Category

Each unicode code point belongs to a category (look at Section [*4.5 General
Category* of the Unicode Standard](http://www.unicode.org/versions/Unicode15.0.0/ch04.pdf#G124142))

| Category | Subcategory | Description                         |
|----------|-------------|-------------------------------------|
| L        | Lu          | Letter, uppercase                   |
| L        | Ll          | Letter, lowercase                   |
| L        | Lt          | Letter, titlecase                   |
| L        | Lm          | Letter, modifier                    |
| L        | Lo          | Letter, other                       |
|          |             |                                     |
| M        | Mn          | Mark, nonspacing                    |
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