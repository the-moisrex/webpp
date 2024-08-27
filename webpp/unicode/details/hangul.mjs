// From https://www.unicode.org/versions/Unicode15.1.0/ch03.pdf#G56669

/// SBase in the standard:
export const hangul_syllable_base = 0xac00n;
/// LBase in the standard:
export const hangul_leading_base = 0x1100n;
/// VBase in the standard:
export const hangul_vowel_base = 0x1161n;
/// TBase in the standard:
export const hangul_trailing_base = 0x11a7n;
/// LCount in the standard:
export const hangul_leading_count = 19n;
/// VCount in the standard:
export const hangul_vowel_count = 21n;
/// TCount in the standard:
export const hangul_trailing_count = 28n;
/// Total count of Hangul blocks and syllables
/// NCount in the standard:
export const hangul_block_count = hangul_vowel_count * hangul_trailing_count;
/// SCount in the standard:
export const hangul_syllable_count = hangul_leading_count * hangul_block_count;

export const isHangul = (codePoint) => {
    return (
        codePoint >= hangul_syllable_base &&
        codePoint < hangul_syllable_base + hangul_syllable_count
    );
};
