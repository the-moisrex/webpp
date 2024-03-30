# Internationalized Domain Names in Applications (IDNA)

Support for the Internationalized Domain Names in
Applications (IDNA) protocol as specified in [RFC 5891](https://tools.ietf.org/html/rfc5891). This is the latest version
of the protocol and is sometimes referred to as **IDNA 2008**.

This library also provides support for Unicode Technical
Standard 46, [Unicode IDNA Compatibility Processing](https://unicode.org/reports/tr46/).

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

Refer to [UTS #46](https://www.unicode.org/reports/tr46/#STD3_Rules).

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
