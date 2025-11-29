
MAKETRANSTABLE(a, あ, ア),
MAKETRANSTABLE(i, い, イ),
MAKETRANSTABLE(yi, い, イ), /* 特殊 */
MAKETRANSTABLE(u, う, ウ),
MAKETRANSTABLE(wu, う, ウ), /* 特殊 */
MAKETRANSTABLE(whu, う, ウ), /* 特殊 */
MAKETRANSTABLE(e, え, エ),
MAKETRANSTABLE(o, お, オ),

/* か行 (cも含む) */
MAKETRANSTABLE(ka, か, カ),
MAKETRANSTABLE(ca, か, カ),
MAKETRANSTABLE(ki, き, キ),
MAKETRANSTABLE(ku, く, ク),
MAKETRANSTABLE(cu, く, ク),
MAKETRANSTABLE(qu, く, ク), /* IMEによってはく */
MAKETRANSTABLE(ke, け, ケ),
MAKETRANSTABLE(ko, こ, コ),
MAKETRANSTABLE(co, こ, コ),

/* さ行 (shi/si) */
MAKETRANSTABLE(sa, さ, サ),
MAKETRANSTABLE(shi, し, シ),
MAKETRANSTABLE(si, し, シ),
MAKETRANSTABLE(ci, し, シ), /* MS-IME等 */
MAKETRANSTABLE(su, す, ス),
MAKETRANSTABLE(se, せ, セ),
MAKETRANSTABLE(ce, せ, セ), /* MS-IME等 */
MAKETRANSTABLE(so, そ, ソ),

/* た行 (chi/ti, tsu/tu) */
MAKETRANSTABLE(ta, た, タ),
MAKETRANSTABLE(chi, ち, チ),
MAKETRANSTABLE(ti, ち, チ),
MAKETRANSTABLE(tsu, つ, ツ),
MAKETRANSTABLE(tu, つ, ツ),
MAKETRANSTABLE(te, て, テ),
MAKETRANSTABLE(to, と, ト),

/* な行 */
MAKETRANSTABLE(na, な, ナ),
MAKETRANSTABLE(ni, に, ニ),
MAKETRANSTABLE(nu, ぬ, ヌ),
MAKETRANSTABLE(ne, ね, ネ),
MAKETRANSTABLE(no, の, ノ),

/* は行 (fu/hu) */
MAKETRANSTABLE(ha, は, ハ),
MAKETRANSTABLE(hi, ひ, ヒ),
MAKETRANSTABLE(fu, ふ, フ),
MAKETRANSTABLE(hu, ふ, フ),
MAKETRANSTABLE(he, へ, ヘ),
MAKETRANSTABLE(ho, ほ, ホ),

/* ま行 */
MAKETRANSTABLE(ma, ま, マ),
MAKETRANSTABLE(mi, み, ミ),
MAKETRANSTABLE(mu, む, ム),
MAKETRANSTABLE(me, め, メ),
MAKETRANSTABLE(mo, も, モ),

/* や行 */
MAKETRANSTABLE(ya, や, ヤ),
MAKETRANSTABLE(yu, ゆ, ユ),
MAKETRANSTABLE(yo, よ, ヨ),

/* ら行 */
MAKETRANSTABLE(ra, ら, ラ),
MAKETRANSTABLE(ri, り, リ),
MAKETRANSTABLE(ru, る, ル),
MAKETRANSTABLE(re, れ, レ),
MAKETRANSTABLE(ro, ろ, ロ),

/* わ行 */
MAKETRANSTABLE(wa, わ, ワ),
MAKETRANSTABLE(wyi, ゐ,	ヰ),
MAKETRANSTABLE(wye, ゑ, ヱ),
MAKETRANSTABLE(wo, を, ヲ),
MAKETRANSTABLE(nn, ん, ン),
MAKETRANSTABLE(xn, ん, ン), /* 特殊 */

/* === 濁音 (Dakuten & Variants) === */
/* が行 */
MAKETRANSTABLE(ga, が, ガ),
MAKETRANSTABLE(gi, ぎ, ギ),
MAKETRANSTABLE(gu, ぐ, グ),
MAKETRANSTABLE(ge, げ, ゲ),
MAKETRANSTABLE(go, ご, ゴ),

/* ざ行 (ji/zi) */
MAKETRANSTABLE(za, ざ, ザ),
MAKETRANSTABLE(ji, じ, ジ), /* ヘボン式 */
MAKETRANSTABLE(zi, じ, ジ), /* 訓令式・日本式 */
MAKETRANSTABLE(zu, ず, ズ),
MAKETRANSTABLE(ze, ぜ, ゼ),
MAKETRANSTABLE(zo, ぞ, ゾ),

/* だ行 (ji/di, zu/du) */
MAKETRANSTABLE(da, だ, ダ),
MAKETRANSTABLE(di, ぢ, ヂ), /* 日本式 */
MAKETRANSTABLE(du, づ, ヅ), /* 日本式 */
MAKETRANSTABLE(dzu, づ, ヅ), /* 別表記 */
MAKETRANSTABLE(de, で, デ),
MAKETRANSTABLE(do, ど, ド),

/* ば行 */
MAKETRANSTABLE(ba, ば, バ),
MAKETRANSTABLE(bi, び, ビ),
MAKETRANSTABLE(bu, ぶ, ブ),
MAKETRANSTABLE(be, べ, ベ),
MAKETRANSTABLE(bo, ぼ, ボ),

/* う゛(Vu) */
MAKETRANSTABLE(vu, う゛, ヴ),

/* === 半濁音 (Handakuten) === */
MAKETRANSTABLE(pa, ぱ, パ),
MAKETRANSTABLE(pi, ぴ, ピ),
MAKETRANSTABLE(pu, ぷ, プ),
MAKETRANSTABLE(pe, ぺ, ペ),
MAKETRANSTABLE(po, ぽ, ポ),

/* === 拗音 (Yoon & Variants) === */
/* きゃ行 */
MAKETRANSTABLE(kya, きゃ, キャ),
MAKETRANSTABLE(kyu, きゅ, キュ),
MAKETRANSTABLE(kyo, きょ, キョ),

/* しゃ行 (sha/sya) */
MAKETRANSTABLE(sha, しゃ, シャ), /* ヘボン式 */
MAKETRANSTABLE(sya, しゃ, シャ), /* 訓令式 */
MAKETRANSTABLE(shu, しゅ, シュ),
MAKETRANSTABLE(syu, しゅ, シュ),
MAKETRANSTABLE(sho, しょ, ショ),
MAKETRANSTABLE(syo, しょ, ショ),

/* ちゃ行 (cha/tya/cya) */
MAKETRANSTABLE(cha, ちゃ, チャ), /* ヘボン式 */
MAKETRANSTABLE(tya, ちゃ, チャ), /* 訓令式 */
MAKETRANSTABLE(cya, ちゃ, チャ), /* IME拡張 */
MAKETRANSTABLE(chu, ちゅ, チュ),
MAKETRANSTABLE(tyu, ちゅ, チュ),
MAKETRANSTABLE(cyu, ちゅ, チュ),
MAKETRANSTABLE(cho, ちょ, チョ),
MAKETRANSTABLE(tyo, ちょ, チョ),
MAKETRANSTABLE(cyo, ちょ, チョ),

/* にゃ行 */
MAKETRANSTABLE(nya, にゃ, ニャ),
MAKETRANSTABLE(nyu, にゅ, ニュ),
MAKETRANSTABLE(nyo, にょ, ニョ),

/* ひゃ行 */
MAKETRANSTABLE(hya, ひゃ, ヒャ),
MAKETRANSTABLE(hyu, ひゅ, ヒュ),
MAKETRANSTABLE(hyo, ひょ, ヒョ),

/* みゃ行 */
MAKETRANSTABLE(mya, みゃ, ミャ),
MAKETRANSTABLE(myu, みゅ, ミュ),
MAKETRANSTABLE(myo, みょ, ミョ),

/* りゃ行 */
MAKETRANSTABLE(rya, りゃ, リャ),
MAKETRANSTABLE(ryu, りゅ, リュ),
MAKETRANSTABLE(ryo, りょ, リョ),

/* ぎゃ行 */
MAKETRANSTABLE(gya, ぎゃ, ギャ),
MAKETRANSTABLE(gyu, ぎゅ, ギュ),
MAKETRANSTABLE(gyo, ぎょ, ギョ),

/* じゃ行 (ja/jya/zya) */
MAKETRANSTABLE(ja, じゃ, ジャ), /* ヘボン式 */
MAKETRANSTABLE(jya, じゃ, ジャ), /* 混合 */
MAKETRANSTABLE(zya, じゃ, ジャ), /* 訓令式 */
MAKETRANSTABLE(ju, じゅ, ジュ),
MAKETRANSTABLE(jyu, じゅ, ジュ),
MAKETRANSTABLE(zyu, じゅ, ジュ),
MAKETRANSTABLE(jo, じょ, ジョ),
MAKETRANSTABLE(jyo, じょ, ジョ),
MAKETRANSTABLE(zyo, じょ, ジョ),

/* ぢゃ行 (dya) */
MAKETRANSTABLE(dya, ぢゃ, ヂャ),
MAKETRANSTABLE(dyu, ぢゅ, ヂュ),
MAKETRANSTABLE(dyo, ぢょ, ヂョ),

/* びゃ行 */
MAKETRANSTABLE(bya, びゃ, ビャ),
MAKETRANSTABLE(byu, びゅ, ビュ),
MAKETRANSTABLE(byo, びょ, ビョ),

/* ぴゃ行 */
MAKETRANSTABLE(pya, ぴゃ, ピャ),
MAKETRANSTABLE(pyu, ぴゅ, ピュ),
MAKETRANSTABLE(pyo, ぴょ, ピョ),

/* === 小書き文字・促音 (Small chars & Variants - x/l) === */
/* あ行小 */
MAKETRANSTABLE(xa, ぁ, ァ),
MAKETRANSTABLE(la, ぁ, ァ),
MAKETRANSTABLE(xi, ぃ, ィ),
MAKETRANSTABLE(li, ぃ, ィ),
MAKETRANSTABLE(lyi, ぃ, ィ),
MAKETRANSTABLE(xu, ぅ, ゥ),
MAKETRANSTABLE(lu, ぅ, ゥ),
MAKETRANSTABLE(xe, ぇ, ェ),
MAKETRANSTABLE(le, ぇ, ェ),
MAKETRANSTABLE(lye, ぇ, ェ),
MAKETRANSTABLE(xo, ぉ, ォ),
MAKETRANSTABLE(lo, ぉ, ォ),

/* っ (tsu/tu variant for small) */
MAKETRANSTABLE(xtu, っ, ッ),
MAKETRANSTABLE(ltu, っ, ッ),
MAKETRANSTABLE(xtsu, っ, ッ),
MAKETRANSTABLE(ltsu, っ, ッ),

/* や行小 */
MAKETRANSTABLE(xya, ゃ, ャ),
MAKETRANSTABLE(lya, ゃ, ャ),
MAKETRANSTABLE(xyu, ゅ, ュ),
MAKETRANSTABLE(lyu, ゅ, ュ),
MAKETRANSTABLE(xyo, ょ, ョ),
MAKETRANSTABLE(lyo, ょ, ョ),

/* わ小 */
MAKETRANSTABLE(xwa, ゎ, ヮ),
MAKETRANSTABLE(lwa, ゎ, ヮ),

/* カ・ケ (ヵ/ヶ) */
MAKETRANSTABLE(xka, ヵ, ヵ),
MAKETRANSTABLE(lka, ヵ, ヵ),
MAKETRANSTABLE(xke, ヶ, ヶ),
MAKETRANSTABLE(lke, ヶ, ヶ),

/* === 外来語・拡張表現 (Extended & Variants) === */
/* いぇ */
MAKETRANSTABLE(ye, いぇ, イェ),

/* うぁ行 (wha/wa variants) */
MAKETRANSTABLE(wha, うぁ, ウァ),
MAKETRANSTABLE(wi, うぃ, ウィ), /* 古典的ローマ字・拡張 */
MAKETRANSTABLE(whi, うぃ, ウィ),
MAKETRANSTABLE(we, うぇ, ウェ), /* 古典的ローマ字・拡張 */
MAKETRANSTABLE(whe, うぇ, ウェ),
MAKETRANSTABLE(who, うぉ, ウォ),

/* くぁ行 (kwa/qwa) */
MAKETRANSTABLE(kwa, くぁ, クァ),
MAKETRANSTABLE(qwa, くぁ, クァ),
MAKETRANSTABLE(kwi, くぃ, クィ),
MAKETRANSTABLE(qwi, くぃ, クィ),
MAKETRANSTABLE(kwe, くぇ, クェ),
MAKETRANSTABLE(qwe, くぇ, クェ),
MAKETRANSTABLE(kwo, くぉ, クォ),
MAKETRANSTABLE(qwo, くぉ, クォ),

/* ぐぁ行 (gwa) */
MAKETRANSTABLE(gwa, ぐぁ, グァ),
MAKETRANSTABLE(gwi, ぐぃ, グィ),
MAKETRANSTABLE(gwe, ぐぇ, グェ),
MAKETRANSTABLE(gwo, ぐぉ, グォ),

/* つぁ行 (tsa/tsu~) */
MAKETRANSTABLE(tsa, つぁ, ツァ),
MAKETRANSTABLE(tsi, つぃ, ツィ),
MAKETRANSTABLE(tse, つぇ, ツェ),
MAKETRANSTABLE(tso, つぉ, ツォ),

/* ちぇ行 (che usually maps to ちぇ, she to しぇ) */
MAKETRANSTABLE(che, ちぇ, チェ),
MAKETRANSTABLE(she, しぇ, シェ),
MAKETRANSTABLE(je, じぇ, ジェ),

/* てぃ・とぅ・でぃ・どぅ (ti/tu/di/du extended) */
MAKETRANSTABLE(thi, てぃ, ティ),
MAKETRANSTABLE(thu, とぅ, トゥ),
MAKETRANSTABLE(tyu, てゅ, テュ), /* ※てゅ */
MAKETRANSTABLE(dhi, でぃ, ディ),
MAKETRANSTABLE(dhu, どぅ, ドゥ),
MAKETRANSTABLE(dyu, でゅ, デュ), /* ※でゅ */

/* ふぁ行 (fa/fi/fe/fo/fyu) */
MAKETRANSTABLE(fa, ふぁ, ファ),
MAKETRANSTABLE(fi, ふぃ, フィ),
MAKETRANSTABLE(fe, ふぇ, フェ),
MAKETRANSTABLE(fo, ふぉ, フォ),
MAKETRANSTABLE(fyu, ふゅ, フュ),

/* う゛ぁ行 (va/vi/ve/vo/vyu) */
MAKETRANSTABLE(va, う゛ぁ, ヴァ),
MAKETRANSTABLE(vi, う゛ぃ, ヴィ),
MAKETRANSTABLE(ve, う゛ぇ, ヴェ),
MAKETRANSTABLE(vo, う゛ぉ, ヴォ),
MAKETRANSTABLE(vyu, う゛ゅ, ヴュ),

/* ひぇ */
MAKETRANSTABLE(hye, ひぇ, ヒェ),

/* 記号 */
//FIX: このような記号も扱えるようにする


{ std::wstring(1,(wchar_t)VK_OEM_COMMA), {L"，", L"，"} },
{ std::wstring(1,(wchar_t)VK_OEM_PERIOD), {L"。", L"。"} },
{ std::wstring(1,(wchar_t)VK_OEM_MINUS), {L"ー", L"ー"} },