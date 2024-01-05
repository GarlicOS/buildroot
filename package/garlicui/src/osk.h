#ifndef OSK_H
#define OSK_H

#define OSK_COLS_COUNT 11
#define OSK_ROWS_COUNT 4
#define OSK_GRID_SIZE OSK_COLS_COUNT * OSK_ROWS_COUNT
#define OSK_GRID_COUNT 3

/**
 * @brief A data structure representing the on-screen keyboard context.
 */
struct osk_context
{
    int grid; /** The on-screen keyboard grid */
	int index; /** The on-screen keyboard index */
    bool lock; /** Whether to lock the on-screen keyboard buffer */
    bool sync; /** Whether to sync the on-screen keyboard buffer */
    bool apply; /** Whether to apply the on-screen keyboard buffer */
    char * buffer; /** The on-screen keyboard buffer */
};

static const char * symbols_page1_grid[] = {
    "1","2","3","4","5","6","7","8","9","0","⇦",
    "!","\"","#","$","%","&","'","*","(",")","⏎",
    "+",",","-","~","/",":",";","=","<",">","⇩",
    "?","@","[","\\","]","^","_","|","{","}","⊕"
};

static const char * uppercase_grid[] = {
    "1","2","3","4","5","6","7","8","9","0","⇦",
    "Q","W","E","R","T","Y","U","I","O","P","⏎",
    "A","S","D","F","G","H","J","K","L","+","⇩",
    "Z","X","C","V","B","N","M"," ","_","/","⊕"
};

static const char * lowercase_grid[] = {
    "1","2","3","4","5","6","7","8","9","0","⇦",
    "q","w","e","r","t","y","u","i","o","p","⏎",
    "a","s","d","f","g","h","j","k","l","@","⇧",
    "z","x","c","v","b","n","m"," ","-",".","⊕"
};

#ifdef USE_EXTRA_GRIDS
static const char * hiragana_page1_grid[] = {
    "あ","い","う","え","お","ら","り","る","れ","ろ","⇦",
    "か","き","く","け","こ","が","ぎ","ぐ","げ","ご","⏎",
    "さ","し","す","せ","そ","ざ","じ","ず","ぜ","ぞ","⇧",
    "た","ち","つ","て","と","だ","ぢ","づ","で","ど","⊕"
};

static const char * hiragana_page2_grid[] = {
    "な","に","ぬ","ね","の","ば","び","ぶ","べ","ぼ","⇦",
    "は","ひ","ふ","へ","ほ","ぱ","ぴ","ぷ","ぺ","ぽ","⏎",
    "ま","み","む","め","も","ん","っ","ゃ","ゅ","ょ","⇧",
    "や","ゆ","よ","わ","を","ぁ","ぃ","ぅ","ぇ","ぉ","⊕"
};

static const char * katakana_page1_grid[] = {
    "ア","イ","ウ","エ","オ","ラ","リ","ル","レ","ロ","⇦",
    "カ","キ","ク","ケ","コ","ガ","ギ","グ","ゲ","ゴ","⏎",
    "サ","シ","ス","セ","ソ","ザ","ジ","ズ","ゼ","ゾ","⇧",
    "タ","チ","ツ","テ","ト","ダ","ヂ","ヅ","デ","ド","⊕"
};

static const char * katakana_page2_grid[] = {
    "ナ","ニ","ヌ","ネ","ノ","バ","ビ","ブ","ベ","ボ","⇦",
    "ハ","ヒ","フ","ヘ","ホ","パ","ピ","プ","ペ","ポ","⏎",
    "マ","ミ","ム","メ","モ","ン","ッ","ャ","ュ","ョ","⇧",
    "ヤ","ユ","ヨ","ワ","ヲ","ァ","ィ","ゥ","ェ","ォ","⊕"
};

static const char * korean_page1_grid[] = {
    "1","2","3","4","5","6","7","8","9","0","⇦",
    "ㅂ","ㅈ","ㄷ","ㄱ","ㅅ","ㅛ","ㅕ","ㅑ","ㅐ","ㅔ","⏎",
    "ㅁ","ㄴ","ㅇ","ㄹ","ㅎ","ㅗ","ㅓ","ㅏ","ㅣ","ㅒ","ㅖ",
    "ㅋ","ㅌ","ㅊ","ㅍ","ㅠ","ㅜ","ㅡ"," ",",",".","⊕"
};
#endif // USE_EXTRA_GRIDS

static const char ** osk_grids[] = {
    symbols_page1_grid,
    uppercase_grid,
    lowercase_grid,
#ifdef USE_EXTRA_GRIDS
    hiragana_page1_grid,
    hiragana_page2_grid,
    katakana_page1_grid,
    katakana_page2_grid,
    korean_page1_grid,
#endif // USE_EXTRA_GRIDS
};

#endif
