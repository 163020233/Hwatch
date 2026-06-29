/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

/**
 * @file geometry_math_stroke.h
 * @brief Defines
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_GEOERTY_MATH_STROKE_H
#define GRAPHIC_LITE_GEOERTY_MATH_STROKE_H


namespace OHOS {
/**
 * @brief The style of the line end cap.
 */
enum LineCap {
    /** Add straight edges to each end of the line */
    BUTT_CAP,
    /** Add a square cap to each end of the line */
    SQUARE_CAP,
    /** Add a circular cap to each end of the line */
    ROUND_CAP
};

/**
 * @brief The type of corner created when two lines intersect
 */
enum LineJoin {
    /** Create sharp corners */
    MITER_JOIN = 0,
    MITER_JOIN_REVERT = 1,
    /** Create Fillets */
    ROUND_JOIN = 2,
    /** Create bevel */
    BEVEL_JOIN = 3,
    MITER_JOIN_ROUND = 4
};
} // namespace OHOS
#endif
