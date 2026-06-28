/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

#ifndef MAP_RENDER_INTERFACE_H
#define MAP_RENDER_INTERFACE_H
#include <cstdint>
#include "gfx_utils/diagram/common/paint.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/rect.h"
#include "gfx_utils/color.h"

namespace OHOS {

class IMapRender {
public:
    /**
    * @brief ÎÄ×ÖÑùÊ½
    */
    typedef struct {
        const char* fontName;             //!< ×Ö¿âÎÄ¼þÃû
        uint32_t fontSize;                //!< ×ÖÌå´óÐ¡
        int16_t letterSpace;              //!< ×ÖÌå¼ä¾à
        ColorType color;                  //!< ×ÖÌåÑÕÉ«
    } TextStyle;

    typedef struct {
        uint32_t radius;                  //!< µã°ë¾¶
        ColorType color;                  //!< »­±ÊÑÕÉ«£¬ARGB
    } PointStyle;

    /**
    * @brief Ìî³äÀàÐÍ
    */
    enum class PaintType {
        PAINT_TYPE_FILL_ONLY,             //!< ½öÌî³ä¼¸ºÎÍ¼ÐÎ
        PAINT_TYPE_FILL_AND_STROKE,       //!< Ìî³ä¼¸ºÎÍ¼ÐÎºÍÃè±ß
        PAINT_TYPE_STROKE_ONLY            //!< ½öÃè±ß
    };

    /**
    * @brief Ãè±ßÑùÊ½
    */
    typedef struct {
        uint32_t  strokeWidth;            //!< »­±Ê¿í¶È, µ¥Î»£ºÏñËØ
        ColorType strokeColor;            //!< »­±ÊÑÕÉ«, ARGB
        uint32_t dashCount;               //!< ÐéÏßÄ£Ê½ÊýÁ¿
        float *dashArray;                 //!< ÐéÏßÄ£Ê½Êý×é
        int32_t dashOffset;               //!< ÐéÏßÆ«ÒÆÁ¿
    } StrokeStyle;

    /**
    * @brief ±ÊË¢ÑùÊ½
    */
    typedef struct {
        ColorType fillColor;              //!< »­±ÊÑÕÉ«, ARGB
        StrokeStyle strokeStyle;          //!< Ãè±ßÑùÊ½
        Paint::PaintStyle paintType;      //!< Ìî³äÑùÊ½
    } MapPaintStyle;

    /**
    * @brief Î»Í¼¶¨Òå
    */
    typedef struct {
        ColorMode colorMode;              //!< Î»Í¼¸ñÊ½
        uint8_t compressMode;             //!< Ñ¹Ëõ¸ñÊ½
        uint8_t *buffer;                  //!< Î»Í¼bufferÄÚÈÝ£¬µØÖ·ÐèÒª¿¼ÂÇGPU¶ÔÆëÒªÇó
        uint32_t bufferSize;              //!< Î»Í¼buffer´óÐ¡
        uint32_t width;                   //!< Î»Í¼¿í¶È, µ¥Î»£ºÏñËØ
        uint32_t height;                  //!< Î»Í¼¸ß¶È, µ¥Î»£ºÏñËØ
        uint32_t stride;                  //!< Î»Í¼Ã¿Ò»ÐÐÏñËØËùÕ¼ÓÃµÄÊµ¼Ê×Ö½ÚÊý£¬ÐèÒª¿¼ÂÇGPU¶ÔÆëÒªÇó
        bool preMultiplied;               //!< ÊÇ·ñÒÑ¾­Ô¤³ËalphaÖµ£¬Ö»ÓÐARGB8888²ÅÓÐÐ§
        char *uuid;                       //!< Í¼ÏñµÄÎ¨Ò»±íÊ¾£¬´óÓÚ0ÈÏÎªÊÇÓÐÐ§±êÊ¶£¬²»Ç¿ÖÆÒªÇóÓÐÐ§±êÊ¶
    } BitmapBufferInfo;

    /**
    * @brief Í¼Æ¬ÀàÐÍ¶¨Òå
    */
    enum class ImageType {
        IMG_TYPE_JPEG,                    //!< Jpeg¸ñÊ½
        IMG_TYPE_MAX,                     //!< ÎÞÐ§¸ñÊ½
    };

    /**
    * @brief Î»Í¼¶¨Òå
    */
    typedef struct {
        ImageType imageType;              //!< Í¼ÏñÄÚÈÝ¸ñÊ½
        uint8_t *buffer;                  //!< Í¼ÏñbufferÄÚÈÝ£¬µØÖ·ÐèÒª¿¼ÂÇGPU¶ÔÆëÒªÇó
        uint32_t bufferSize;              //!< Í¼Ïñbuffer´óÐ¡
        char *uuid;                       //!< Í¼ÏñµÄÎ¨Ò»±íÊ¾£¬´óÓÚ0ÈÏÎªÊÇÓÐÐ§±êÊ¶£¬²»Ö§³ÖÎÞÐ§±êÊ¶
    } ImageBufferInfo;

    /**
    * @brief Ìî³äÀàÐÍ
    */
    enum class SvgType {
        SVG_TYPE_SERIALIZED,    // ÐòÁÐ»¯µÄSVGÎÄ¼þ
        SVG_TYPE_MAX,           // ½öÌî³äÃè±ß
    };

    /**
    * @brief Î»Í¼¶¨Òå
    */
    typedef struct {
        SvgType svgType;                 //!< SVGÄÚÈÝ¸ñÊ½
        uint8_t *buffer;                 //!< SVGÎÄ¼þÄÚÈÝ
        uint32_t bufferSize;             //!< SVGÎÄ¼þ´óÐ¡
        char *uuid;                      //!< SVGµÄÎ¨Ò»±íÊ¾£¬´óÓÚ0ÈÏÎªÊÇÓÐÐ§±êÊ¶£¬²»Ö§³ÖÎÞÐ§±êÊ¶
    } SvgBufferInfo;

    virtual ~IMapRender() = default;

    /**
     * @brief Initiate a series of drawing commands
     *
     * @param width the width limit to render the map
     * @param height the height limit to render the map
     * @param mapRect the area(view port) to layout the map
     */
    virtual void BeginDraw(int32_t width, int32_t height, Rect mapRect) = 0;

    /**
     * @brief Submits the current drawing command.
     */
    virtual void CommitDraw() = 0;

    /**
     * @brief Draw a bunch of points
     *
     * @param points the coordinates of the points
     * @param pointNum the number of the points
     * @param style the paint style of the points
     */
    virtual void DrawPoints(Point *points, uint32_t pointNum, PointStyle *style) = 0;

    /**
     * @brief Draw a polyline
     *
     * @param points the coordinates of the breakpoints
     * @param pointNum the number of the breakpoints
     * @param style the paint style of the polyline
     */
    virtual void DrawPolyline(Point *points, uint32_t pointNum, StrokeStyle *style) = 0;

    /**
     * @brief Draw a polygon
     *
     * @param points the coordinates of the breakpoints
     * @param pointNum the number of the breakpoints
     * @param style the paint style of the polygon
     */
    virtual void DrawPolygon(Point *points, uint32_t pointNum, MapPaintStyle *style) = 0;

    /**
     * @brief Draw a rectangle
     *
     * @param rect the area to draw the rectangle
     * @param style the paint style of the rectangle
     * @param angle the clockwise direction rotate angle of the rectangle, the rotation center is the rect center
     */
    virtual void DrawRect(Rect rect, MapPaintStyle *style, float angle) = 0;

    /**
     * @brief Draw a bitmap
     *
     * @param buffer the pixels of image in memory
     * @param rect the area to draw the bitmap
     * @param angle the clockwise direction rotate angle of the bitmap, the rotation center is the rect center
     */
    virtual void DrawBitmap(BitmapBufferInfo *buffer, Rect rect, float angle) = 0;

    /**
     * @brief Draw a image by picture
     *
     * @param image the information of the image file loaded in memory
     * @param rect the area to draw the image
     * @param angle the clockwise direction rotate angle of the image, the rotation center is the rect center
     */
    virtual void DrawImage(ImageBufferInfo *image, Rect rect, float angle) = 0;

    /**
     * @brief Draw a text line
     *
     * @param center the position(center) to draw the text, and also is the rotation center
     * @param text the character string to draw
     * @param style the paint style of the text
     * @param angle the clockwise direction rotate angle of the text
     */
    virtual void DrawText(Point center, const char *text, TextStyle *style, float angle) = 0;

    /**
     * @brief measure for a text
     *
     * @param text the character string to measure for
     * @param style the style of the text to measure for
     * @param width the addr to save the width of the text
     * @param height the addr to save the height of the text
     */
    virtual bool MeasureText(const char *text, TextStyle *style, int32_t *width, int32_t *height) = 0;

    /**
     * @brief Draw a image by svg
     *
     * @param svg the information of the svg file loaded in memory
     * @param rect the area to draw the svg
     * @param angle the clockwise direction rotate angle of the svg, the rotation center is the rect center
     */
    virtual void DrawSvg(SvgBufferInfo *svg, Rect rect, float angle) = 0;
};

}  // namespace OHOS

#endif