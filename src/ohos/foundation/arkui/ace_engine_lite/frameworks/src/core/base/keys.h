/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_ACELITE_KEYS_H
#define OHOS_ACELITE_KEYS_H

#include "acelite_config.h"

#ifndef KEYWORD
#define KEYWORDS_ENUM_DEFINE
#define KEYWORD(enumkey, keystr) K_##enumkey,

namespace OHOS {
namespace ACELite {
enum {
    K_UNKNOWN, // enum start from 0
#endif         // KEYWORD

    /*
     * Rules to define the KEYWORD:
     * Should not be duplicated,
     * and use xxx_yyy instead the xxx-yyy as enumstr
     * because of enum define limitation
     */
    // clang-format off
    // Reused by the struct begin
    // Sample:KEYWORD(enum_string, real-key-string)
    KEYWORD(ALIGN_ITEMS, alignItems) // layout style
    KEYWORD(ANALOG_CLOCK, analog-clock) // tag name
    KEYWORD(ANIMATION_DELAY, animationDelay) // animation delay
    KEYWORD(ANIMATION_DURATION, animationDuration) // animation during
    KEYWORD(ANIMATION_FILL_MODE, animationFillMode) // animation fill mode
    KEYWORD(ANIMATION_ITERATION_COUNT, animationIterationCount) // animation iteration count
    KEYWORD(ANIMATION_NAME, animationName) // animation name
    KEYWORD(ANIMATION_TIMING_FUNCTION, animationTimingFunction) // animation timing function
    KEYWORD(ARC, arc) // circle progress
    KEYWORD(AUTOPLAY, autoplay) // video component autoplay attribute
    KEYWORD(BACKGROUND_COLOR, backgroundColor) // chart line color
    KEYWORD(BACKGROUND_IMAGE, backgroundImage) // common style, but only button, checkbox, radio support
    KEYWORD(BLOCK_COLOR, blockColor)
    KEYWORD(BORDER_BOTTOM_COLOR, borderBottomColor) // common style border bottom color
    KEYWORD(BORDER_LEFT_COLOR, borderLeftColor) // common style border left color
    KEYWORD(BORDER_RIGHT_COLOR, borderRightColor) // common style border right color
    KEYWORD(BORDER_TOP_COLOR, borderTopColor) // common style border top color
    KEYWORD(BORDER_COLOR, borderColor) // common style, horizon progress style
    KEYWORD(BORDER_RADIUS, borderRadius) // horizon progress style
    KEYWORD(BORDER_BOTTOM_WIDTH, borderBottomWidth) // common style border bottom width
    KEYWORD(BORDER_LEFT_WIDTH, borderLeftWidth) // common style border left width
    KEYWORD(BORDER_RIGHT_WIDTH, borderRightWidth) // common style border right width
    KEYWORD(BORDER_TOP_WIDTH, borderTopWidth) // common style border top width
    KEYWORD(BORDER_WIDTH, borderWidth) // common style, horizon progress style
    KEYWORD(BREAK, break) // text overflow type
    KEYWORD(CAMERA, camera) // camera component
    KEYWORD(CANVAS, canvas) // tag name
    KEYWORD(CANVASGRADIENT, canvasGradient) // gradient fillstyle or strokestyle
    KEYWORD(CANVASPATTERN, canvasPattern) // pattern fillstyle or strokestyle
    KEYWORD(CENTER, center) // text align type
    KEYWORD(CENTER_X, centerX) // circle progress x
    KEYWORD(CENTER_Y, centerY) // circle progress y
    KEYWORD(CHANGE, change) // the input component event name, picker-view event name
    KEYWORD(CHECKED, checked) // the input and switch component of checkbox attribute
    KEYWORD(CHECKBOX, checkbox) // the input component type value
    KEYWORD(CLICK, click) // click event listener
    KEYWORD(CLIP, clip) // text overflow type
    KEYWORD(CHART, chart) // tag name
    KEYWORD(CLOCK_HAND, clock-hand) // tag name
    KEYWORD(COLOR, color) // text style type, rectangular clock-hand fill color, horizon progress style
    KEYWORD(COLUMN, column) // layout style
    KEYWORD(COLUMN_REVERSE, column-reverse) // layout style
    KEYWORD(CONTAIN, contain) // image component contain
    KEYWORD(CONTROLS, controls) // video component controls attribute
    KEYWORD(COVER, cover) // image component cover
    KEYWORD(DATASETS, datasets) // chart data
    KEYWORD(DATE, date) // picker-view attr value
    KEYWORD(DAY, day)
    KEYWORD(DIGIT2, digit2)
    KEYWORD(DIRECTION, direction) // progress direction
    KEYWORD(DISPLAY, display) // div layout
    KEYWORD(DIV, div) // tag name
    KEYWORD(DRAG, drag) // drag event listener
    KEYWORD(DURATION, duration)
    KEYWORD(EASE_IN, ease-in) // animation time function value
    KEYWORD(EASE_IN_OUT, ease-in-out) // animation time function value
    KEYWORD(EASE_OUT, ease-out) // animation time function value
    KEYWORD(ELLIPSIS, ellipsis) // text overflow type
    KEYWORD(EDITTEXTTYPE, editTextType) // text overflow type
    KEYWORD(END, end) // date picker-view attr
    KEYWORD(ERROR, error) // camera component error event
    KEYWORD(EXPAND, expand) // text overflow type
    KEYWORD(AUTO_WIDTH, autoWidth) // text overflow type
    KEYWORD(FALSE, false) // common attributes
    KEYWORD(FINISH, finish) // video component finish event tag
    KEYWORD(FILL, fill) // image component fill
    KEYWORD(FILL_MODE, fillmode) // image-animator attribtue
    KEYWORD(FIT_ORIGINAL_SIZE, fitOriginalSize) // image component fit original size
    KEYWORD(FIXED_SIZE, fixedsize) // image-animator attribute
    KEYWORD(FLEX_DIRECTION, flexDirection) // layout style
    KEYWORD(FLEX_END, flex-end) // layout style
    KEYWORD(FLEX_START, flex-start) // layout style
    KEYWORD(FLEX_WRAP, flexWrap) // layout style
    KEYWORD(FONT_SIZE, fontSize) // text style type
    KEYWORD(FONT_FAMILY, fontFamily) // text style type
    KEYWORD(FORWARDS, forwards) // animation fill value
    KEYWORD(HEIGHT, height) // common attributes
    KEYWORD(HOUR, hour) // analog-clock hour
    KEYWORD(HOUR12, hour12)
    KEYWORD(ID, id) // common attributes
    KEYWORD(IMAGE, image) // tag name
    KEYWORD(IMAGE_ANIMATOR, image-animator) // tag name
    KEYWORD(IMAGES, images) // image-animator attribute
    KEYWORD(INDEX, index) // swiper attr index
    KEYWORD(INPUT, input) // input component name
    KEYWORD(ITEM_SELECTED, itemselected) // scroll item selected event listener
    KEYWORD(ITERATION, iteration) // image-animator attribute
    KEYWORD(JUSTIFY_CONTENT, justifyContent) // layout style
    // ohos begin
    // list-item adaptive
    KEYWORD(LONG_TEXT, longText) // common attributes
    // ohos end
    KEYWORD(KEY, key) // onkey event
    KEYWORD(LABELS, labels) // chart labels
    KEYWORD(LARGER, larger) // text font size type
    KEYWORD(LEFT, left) // text align type
    KEYWORD(LETTER_SPACING, letterSpacing) // text style type
    KEYWORD(LINE_HEIGHT, lineHeight) // text style type
    KEYWORD(LIST, list) // list tag name
    KEYWORD(LIST_ITEM, list-item) // list-item tag name
    // ohos begin
    // ace, add scale list
    KEYWORD(ARCLIST_ITEM, arclist-item) // arclist-item tag name
    KEYWORD(ARCLIST, arclist) // arclist tag name
    // ace, add scroll view
    KEYWORD(SCROLL, scroll) // scroll tag name
    // ohos end
    KEYWORD(LONG, long)
    KEYWORD(LONGPRESS, longpress) // long progress event listener
    KEYWORD(LOOP, loop) // swiper loop attribute
    KEYWORD(MARGIN, margin) // common style margin
    KEYWORD(MARGIN_BOTTOM, marginBottom) // common style margin
    KEYWORD(MARGIN_LEFT, marginLeft) // common style margin left
    KEYWORD(MARGIN_RIGHT, marginRight) // common style margin right
    KEYWORD(MARGIN_TOP, marginTop) // common style margin top
    KEYWORD(MARQUEE, marquee) // marquee tag name
    KEYWORD(MAX, max) // common attributes
    KEYWORD(MAX_LENGTH, maxlength) // maxlength of input
    KEYWORD(MIN, min) // common attributes, analog-clock minute
    KEYWORD(MINI, mini) // text font size type
    KEYWORD(MINUTE, minute)
    KEYWORD(MONTH, month)
    KEYWORD(MUTED, muted) // video component muted attribute
    KEYWORD(NAME, name) // the attribute name
    KEYWORD(NONE, none) // animation fill value
    KEYWORD(NO_REPEAT, no-repeat) // no-repeat pattern type
    KEYWORD(NUMERIC, numeric)
    KEYWORD(OBJECT_FIT, objectFit) // image component object-fit
    KEYWORD(OPACITY, opacity) // common style opacity
    KEYWORD(OPTIONS, options) // chart options
    KEYWORD(PADDING, padding)
    KEYWORD(PADDING_BOTTOM, paddingBottom)
    KEYWORD(PADDING_LEFT, paddingLeft)
    KEYWORD(PADDING_RIGHT, paddingRight)
    KEYWORD(PADDING_TOP, paddingTop)
    KEYWORD(PASSWORD, password) // type of input component
    KEYWORD(PERCENT, percent)
    KEYWORD(PICKER_VIEW, picker-view) // tag name
    KEYWORD(REF, ref) // common attribute
    KEYWORD(PREPARED, prepared) // video component prepared event
    KEYWORD(PIVOT_X, pivotX) // clock-hand pivot-x
    KEYWORD(PIVOT_Y, pivotY) // clock-hand pivot-y
    KEYWORD(PLACEHOLDER, placeholder) // placeholder of edittext
    KEYWORD(PLACEHOLDER_COLOR, placeholderColor) // placeholder-color of edittext
    KEYWORD(PROGRESS, progress) // progress tag name
    // ohos begin
    // ace, add text rotation
    KEYWORD(ORIENTATION, text-orientation) // text rotation
    // ohos end
    KEYWORD(QRCODE, qrcode) // qrcode tag name
    KEYWORD(RADIO, radio) // the type in input
    KEYWORD(RADIUS, radius) // circle progress radius
    KEYWORD(RANGE, range) // picker-view attr
    KEYWORD(PAUSE, pause) // video component pause event tag
    KEYWORD(REVERSE, reverse) // image-animator attribute
    KEYWORD(REPEAT, repeat) // repeat pattern type
    KEYWORD(REPEAT_X, repeat-x) // repeat-x pattern type
    KEYWORD(REPEAT_Y, repeat-y) // repeat-y pattern type
    KEYWORD(RIGHT, right) // text align type
    KEYWORD(ROTATE, rotate) // animation rotate
    KEYWORD(ROW, row) // layout style
    KEYWORD(ROW_REVERSE, row-reverse) // layout style
    KEYWORD(SCALE_DOWN, scaleDown) // image component scale down
    KEYWORD(SCROLLAMOUNT, scrollamount) // marquee scroll speed
    KEYWORD(SCROLLBOTTOM, scrollbottom) // scroll bottom event listener
    KEYWORD(SCROLLDELAY, scrolldelay) // marquee scroll delay
    KEYWORD(SCROLLEND, scrollend) // scroll end event listener
    KEYWORD(SCROLLSTART, scrollstart) // scroll start event listener
    KEYWORD(SCROLLTOP, scrolltop) // scroll top event listener
	// ohos begin
    // ace, add progress bar
    KEYWORD(SCROLLBAR, scrollbar)
    // ohos end
    KEYWORD(SEC, sec) // analog-clock second
    KEYWORD(SECOND, second)
    KEYWORD(SEEKING, seeking) // // video component seeking event tag
    KEYWORD(SEEKED, seeked) // // video component seeked event tag
    KEYWORD(SELECTED, selected) // picker-view attr
    KEYWORD(SELECTED_COLOR, selectedColor) // picker-view style
    KEYWORD(SELECTED_FONT_FAMILY, selectedFontFamily) // picker-view style
    KEYWORD(SELECTED_FONT_SIZE, selectedFontSize) // picker-view style
    KEYWORD(SHORT, short)
    KEYWORD(SHOW, show)
    KEYWORD(SLIDER, slider) // tag name
    KEYWORD(SPACE_AROUND, space-around) // layout style
    KEYWORD(SPACE_BETWEEN, space-between) // layout style
    KEYWORD(SPACE_EVENLY, space-evenly) // layout style
    KEYWORD(SRC, src) // common attributes, use for image view, video compnent
    KEYWORD(IMAGE_WIDTH, width) // common attributes, use for image view, video compnent
    KEYWORD(IMAGE_HEIGHT, height) // common attributes, use for image view, video compnent
    KEYWORD(STACK, stack) // tag name
    KEYWORD(STANDARD, standard) // text font size type
    KEYWORD(STOP, stop) // image-animator stop event, video component end event tag
    KEYWORD(START, start) // date picker-view attr, video component start event tag
    KEYWORD(START_ANGLE, startAngle) // circle progress start angle
    KEYWORD(STEP, step)
    KEYWORD(STROKE_WIDTH, strokeWidth) // horizon/arc progress style
    KEYWORD(SWIPE, swipe) // swipe event listener
    KEYWORD(SWIPER, swiper) // tag name
    KEYWORD(SWITCH, switch) // tag name
    KEYWORD(TAB_BAR, tab-bar) // tag name
    KEYWORD(TAB_CONTENT, tab-content) // tag name
    KEYWORD(TABS, tabs) // tag name
    KEYWORD(TEXT, text) // tag name, picker-view attr value
    KEYWORD(TEXT_ALIGN, textAlign) // text style type
    // ohos begin
    // ace text style vertical type
    KEYWORD(TEXT_ALIGN_VERTICAL, textAlignVertical) // text style vertical type
    // ohos end
    KEYWORD(TEXT_OVERFLOW, textOverflow) // text style type
    KEYWORD(TIME, time) // picker-view attr value
    KEYWORD(TIME_UPDATE, timeupdate) // video component timeupdate event tag
    KEYWORD(TOP, top)
    // ohos begin
    // ace text style vertical type
    KEYWORD(BOTTOM, bottom)
    // ohos begin
    KEYWORD(TOTAL_ANGLE, total-angle) // circle progress style
    KEYWORD(TOUCHSTART, touchStart) // touch event listener
    KEYWORD(TOUCHMOVE, touchmove) // touchmove event listener
    KEYWORD(TOUCHEND, touchEnd) // touchend event listener
    KEYWORD(TOUCHCANCEL, touchcancel) // ontouchcancel event
    KEYWORD(TRANSLATE_X, translateX) // animation transform x
    KEYWORD(TRANSLATE_Y, translateY) // animation transform y
    KEYWORD(TRUE, true) // common attributes
    KEYWORD(TYPE, type) // clock-hand type
    KEYWORD(VALUE, value) // common attributes
    KEYWORD(VERTICAL, vertical) // swiper attr vertical
    KEYWORD(VIDEO, video) // video tag name
    KEYWORD(WIDTH, width) // common attributes
    KEYWORD(WRAP, wrap) // flex wrap
    KEYWORD(WEEKDAY, weekday)
    KEYWORD(YEAR, year)
    KEYWORD(STYLE, style)
    KEYWORD(USEGROUP, useGroup)
    KEYWORD(MINIMUMFRACTIONDIGITS, minimumFractionDigits)
    KEYWORD(MAXIMUMFRACTIONDIGITS, maximumFractionDigits)
// ohos begin
// ace text style vertical type
    KEYWORD(BAIDUMAP, baidumap) // baidumap tag name
    KEYWORD(MAP, map) // map tag
// ohos end
    KEYWORD(BACKGROUNDCOLORSPANCOLOR, backgroundcolorspancolor)
    KEYWORD(BACKGROUNDCOLORSPANSTART, backgroundcolorspanstart)
    KEYWORD(BACKGROUNDCOLORSPANEND, backgroundcolorspanend)
    KEYWORD(FOREGROUNDCOLORSPANCOLOR, foregroundcolorspancolor)
    KEYWORD(FOREGROUNDCOLORSPANSTART, foregroundcolorspanstart)
    KEYWORD(FOREGROUNDCOLORSPANEND, foregroundcolorspanend)
    KEYWORD(LINEBACKGROUNDCOLORSPANCOLOR, linebackgroundcolorspancolor)
    KEYWORD(LINEBACKGROUNDCOLORSPANSTART, linebackgroundcolorspanstart)
    KEYWORD(LINEBACKGROUNDCOLORSPANEND, linebackgroundcolorspanend)
    KEYWORD(ABSOLUTESIZESPANSTART, absolutesizespanstart)
    KEYWORD(ABSOLUTESIZESPANEND, absolutesizespanend)
    KEYWORD(ABSOLUTESIZESPANSIZE, absolutesizespansize)
    KEYWORD(RELATIVESIZESPANSTART, relativesizespanstart)
    KEYWORD(RELATIVESIZESPANEND, relativesizespanend)
    KEYWORD(RELATIVESIZESPANSIZE, relativesizespansize)
    KEYWORD(SPANNABLESTYLE, spannablestyle)
    KEYWORD(SPANNABLESTART, spannablestart)
    KEYWORD(SPANNABLEEND, spannableend)
    // Reused by the struct end
// clang-format on
#ifdef KEYWORDS_ENUM_DEFINE
    KEYWORDS_MAX, // max size is UINT16_MAX
};
#undef KEYWORDS_ENUM_DEFINE
#undef KEYWORD
} // namespace ACELite
} // OHOS namespace
#endif // KEYWORDS_ENUM_DEFINE

#endif // OHOS_ACELITE_KEYS_H
