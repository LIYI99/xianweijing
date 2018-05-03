/*!
*****************************************************************************
** \file        software/linux/kernel/drv/media/osd.c
**
** \version     $Id$
**
** \brief
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2015 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/
#include "drv_public.h"

#include "dsp_api.h"
#include "vi_api.h"
#include "vo_pri.h"

static VO_OSD_InfoT gk_osd_info[NUM_FB] =
{
    {
        .vo_id          = {-1, -1},
        .mtx_init       = 0,
        .color_format   = GK_FB_COLOR_UNSUPPORTED,
        .conversion_buf =
        {
            .available      = -1,
            .ping_buf       = NULL,
            .ping_buf_size  = 0,
            .pong_buf       = NULL,
            .pong_buf_size  = 0,
        },
        .xres           = -1,
        .yres           = -1,
        .cmap_changed   = 0,
        .csc_en         = -1,
        .auto_copy      = 0,
        .osd_flip       = GK_VO_FLIP_NORMAL,
    },
    {
        .vo_id          = {-1, -1},
        .mtx_init       = 0,
        .color_format   = GK_FB_COLOR_UNSUPPORTED,
        .conversion_buf =
        {
            .available      = -1,
            .ping_buf       = NULL,
            .ping_buf_size  = 0,
            .pong_buf       = NULL,
            .pong_buf_size  = 0,
        },
        .xres           = -1,
        .yres           = -1,
        .cmap_changed   = 0,
        .csc_en         = -1,
        .auto_copy      = 0,
        .osd_flip       = GK_VO_FLIP_NORMAL,
    },
};

static VO_OSD_SinkCfgT gk_osd_sink_cfg[2] =
{
    {
        .mtx_init   = 0,
        .data_valid = 0,
    },
    {
        .mtx_init   = 0,
        .data_valid = 0,
    },
};

extern int dma_memcpy(u8 *dest_addr, u8 *src_addr, u32 size);

static inline int _fb_memcpy(u8 *dest_addr, u8 *src_addr, u32 size)
{
    return dma_memcpy((u8 *)gk_virt_to_phys((u32)dest_addr),
        (u8 *)gk_virt_to_phys((u32)src_addr),
        size);
}

static inline int _direct_pan_display_arch(struct fb_info *info,
    VO_OSD_InfoT *posd_info, u8 *pDstOSDBuffer)
{
    int                             i, vo_id;
    HAL_DSP_VoOsdBufT   osd_buf_setup;

    osd_buf_setup.osd_buf_dram_addr     = gk_virt_to_phys((u32)pDstOSDBuffer);
    osd_buf_setup.osd_buf_pitch         = info->fix.line_length;
    osd_buf_setup.osd_buf_repeat_field  = 0;

    for (i = 0; i < 2; i++) {
        vo_id = posd_info->vo_id[i];
        if (vo_id == 0 || vo_id == 1) {
            VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD_BUFFER, &osd_buf_setup);
        }
    }

    return 0;
}

static inline int _rgb2clut_pan_display_arch(struct fb_info *info,
    VO_OSD_InfoT *posd_info, u8 *pDstOSDBuffer)
{
    int                             i, vo_id;
    HAL_DSP_VoOsdBufT   osd_buf_setup;

    osd_buf_setup.osd_buf_dram_addr     = gk_virt_to_phys((u32)pDstOSDBuffer);
    osd_buf_setup.osd_buf_pitch         = info->fix.line_length >> 1;
    osd_buf_setup.osd_buf_repeat_field  = 0;

    for (i = 0; i < 2; i++) {
        vo_id = posd_info->vo_id[i];
        if (vo_id == 0 || vo_id == 1) {
            VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD_BUFFER, &osd_buf_setup);
        }
    }

    return 0;
}

static int _rgb2yuv_pan_display_arch(struct fb_info *info,
    VO_OSD_InfoT *posd_info, u16 *pDstOSDBuffer)
{
    int                             i, vo_id;
    HAL_DSP_VoOsdBufT   osd_buf_setup;

    osd_buf_setup.osd_buf_dram_addr = gk_virt_to_phys((u32)pDstOSDBuffer);
    osd_buf_setup.osd_buf_pitch = info->fix.line_length;
    if (posd_info->color_format == GK_FB_COLOR_RGBA8888 ||
        posd_info->color_format == GK_FB_COLOR_ABGR8888) {
        osd_buf_setup.osd_buf_pitch >>= 1;
    }

    osd_buf_setup.osd_buf_repeat_field = 0;

    for (i = 0; i < 2; i++) {
        vo_id = posd_info->vo_id[i];
        if (vo_id == 0 || vo_id == 1) {
            VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD_BUFFER, &osd_buf_setup);
        }
    }

    return 0;
}

static inline int _osd_on_vo_change_pre_arch(VO_OSD_InfoT *posd_info)
{
    if (posd_info) {
        posd_info->support_direct_mode = 1;
        posd_info->support_mixer_csc   = 0;
    }

    return 0;
}

static int _osd_on_vo_change_arch(int vo_id,
    GK_VO_SinkModeT *sink_mode,
    VO_OSD_InfoT *posd_info,
    struct gk_platform_fb *pplatform_info)
{
    int                 ret = 0;
    VO_WindowInfoT active_window;
    int                 width, height;
    HAL_DSP_VoOsdSetupT       def_osd_setup;
    VO_SRC_OsdClutInfoT osd_clut_info;
    int                 blank_x, blank_y;

    /* Disable OSD */
    if (!posd_info) {
        memset(&def_osd_setup, 0, sizeof(def_osd_setup));
        VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD, &def_osd_setup);
        return ret;
    }

    ret = VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_GET_ACTIVE_WIN, &active_window);
    if (ret) {
        vo_err("%s: GK_VIDEO_SOURCE_GET_ACTIVE_WIN %d\n",
                __func__, ret);
        return ret;
    } else {
        width   = active_window.width;
        height  = active_window.end_y - active_window.start_y + 1;
        if (posd_info->interlace[vo_id])
            height <<= 1;
    }

    def_osd_setup.en    = 1;
    def_osd_setup.flip  = sink_mode->osd_flip;
    def_osd_setup.rescaler_en = sink_mode->osd_rescale.enable;
    def_osd_setup.premultiplied = 0;
    def_osd_setup.global_blend = 0xFF;

    if (!def_osd_setup.rescaler_en) {
        def_osd_setup.win_width =
            MIN(width, pplatform_info->screen_var.xres);
        def_osd_setup.win_height =
            MIN(height, pplatform_info->screen_var.yres);
    } else {
        def_osd_setup.win_width = sink_mode->osd_rescale.width;
        def_osd_setup.win_height = sink_mode->osd_rescale.height;
    }

    blank_x = width - def_osd_setup.win_width;
    blank_y = height - def_osd_setup.win_height;

    if (sink_mode->osd_offset.specified) {
        def_osd_setup.win_offset_x =
            MIN(sink_mode->osd_offset.offset_x, blank_x);
        def_osd_setup.win_offset_y =
            MIN(sink_mode->osd_offset.offset_y, blank_y);
    } else {
        def_osd_setup.win_offset_x = blank_x >> 1;
        def_osd_setup.win_offset_y = blank_y >> 1;
    }

    if (!def_osd_setup.rescaler_en) {
        def_osd_setup.rescaler_input_width = 0;
        def_osd_setup.rescaler_input_height = 0;
    } else {
        def_osd_setup.rescaler_input_width =
            MIN(width, pplatform_info->screen_var.xres);;
        def_osd_setup.rescaler_input_height =
            MIN(height, pplatform_info->screen_var.yres);
    }

    def_osd_setup.osd_buf_repeat_field = 0;
    def_osd_setup.osd_buf_dram_addr = pplatform_info->screen_fix.smem_start
        + pplatform_info->screen_fix.line_length * pplatform_info->screen_var.yoffset;
    def_osd_setup.osd_buf_pitch =
        pplatform_info->screen_fix.line_length;

    if (posd_info->interlace[vo_id]) {
        def_osd_setup.win_height >>= 1;
        def_osd_setup.win_offset_y >>= 1;
        def_osd_setup.rescaler_input_height >>= 1;
    }

    switch (pplatform_info->color_format)
    {
    case GK_FB_COLOR_CLUT_8BPP:
        def_osd_setup.src = GK_OSD_SRC_MAPPED_IN;
        def_osd_setup.osd_direct_mode = 0;
        def_osd_setup.osd_transparent_color = 0;
        def_osd_setup.osd_transparent_color_en = 0;
        VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD, &def_osd_setup);

        osd_clut_info.pclut_table = pplatform_info->clut_table;
        osd_clut_info.pblend_table = pplatform_info->blend_table;
        VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD_CLUT, &osd_clut_info);
        break;

    case GK_FB_COLOR_AUTO:
    case GK_FB_COLOR_RGB565:
    case GK_FB_COLOR_BGR565:
        if (posd_info->support_direct_mode) {
            def_osd_setup.src = GK_OSD_SRC_DIRECT_IN;
            def_osd_setup.osd_direct_mode = 0;//UYV565
            def_osd_setup.osd_transparent_color =
                sink_mode->csc_en ? 0x8090 : 0x0; //RGB(0:0:0)
            def_osd_setup.osd_transparent_color_en = 0;
            VO_CORE_Source_Cmd(vo_id,
                GK_VIDEO_SOURCE_SET_OSD, &def_osd_setup);
        } else {
            def_osd_setup.src = GK_OSD_SRC_MAPPED_IN;
            def_osd_setup.osd_direct_mode = 0;
            def_osd_setup.osd_transparent_color = 0;
            def_osd_setup.osd_transparent_color_en = 0;
            def_osd_setup.osd_buf_pitch >>= 1;
            VO_CORE_Source_Cmd(vo_id,
                GK_VIDEO_SOURCE_SET_OSD, &def_osd_setup);

            osd_clut_info.pclut_table = pplatform_info->clut_table;
            osd_clut_info.pblend_table = pplatform_info->blend_table;
            VO_CORE_Source_Cmd(vo_id,
                GK_VIDEO_SOURCE_SET_OSD_CLUT, &osd_clut_info);
        }
        break;

    case GK_FB_COLOR_ARGB4444:
    case GK_FB_COLOR_RGBA4444:
        if (posd_info->support_direct_mode) {
            def_osd_setup.src = GK_OSD_SRC_DIRECT_IN;
            def_osd_setup.osd_direct_mode = 1;//1--AYUV4444
            def_osd_setup.osd_transparent_color =
                sink_mode->csc_en ? 0x8090 : 0x0; //RGB(0:0:0)
            def_osd_setup.osd_transparent_color_en = 0;
            VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD, &def_osd_setup);
        } else {
            def_osd_setup.src = GK_OSD_SRC_MAPPED_IN;
            def_osd_setup.osd_direct_mode = 0;
            def_osd_setup.osd_transparent_color = 0;
            def_osd_setup.osd_transparent_color_en = 0;
            def_osd_setup.osd_buf_pitch >>= 1;
            VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD, &def_osd_setup);

            osd_clut_info.pclut_table = pplatform_info->clut_table;
            osd_clut_info.pblend_table = pplatform_info->blend_table;
            VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD_CLUT, &osd_clut_info);
        }
        break;

    case GK_FB_COLOR_RGBA8888:
    case GK_FB_COLOR_ABGR8888:
        if (posd_info->support_direct_mode) {
            def_osd_setup.src = GK_OSD_SRC_DIRECT_IN;
            if(pplatform_info->convert_format == GK_OSD_COLOR_AYUV1555) {
                def_osd_setup.osd_direct_mode = 2;//1--AYUV4444 2--AYUV1555
            } else {
                def_osd_setup.osd_direct_mode = 1;
            }
            def_osd_setup.osd_transparent_color =
                sink_mode->csc_en ? 0x8090 : 0x0; //RGB(0:0:0)
            def_osd_setup.osd_transparent_color_en = 0;
            def_osd_setup.osd_buf_pitch >>= 1;
            VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD, &def_osd_setup);
        } else {
            def_osd_setup.src = GK_OSD_SRC_MAPPED_IN;
            def_osd_setup.osd_direct_mode = 0;
            def_osd_setup.osd_transparent_color = 0;
            def_osd_setup.osd_transparent_color_en = 0;
            def_osd_setup.osd_buf_pitch >>= 2;
            VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD, &def_osd_setup);

            osd_clut_info.pclut_table = pplatform_info->clut_table;
            osd_clut_info.pblend_table = pplatform_info->blend_table;
            VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_OSD_CLUT, &osd_clut_info);
        }
        break;
    case GK_FB_COLOR_AYUV1555:
        if (posd_info->support_direct_mode) {
            def_osd_setup.src = GK_OSD_SRC_DIRECT_IN;
            def_osd_setup.osd_direct_mode = 2;//AYUV1555
            def_osd_setup.osd_transparent_color =
                sink_mode->csc_en ? 0x8090 : 0x0; //RGB(0:0:0)
            def_osd_setup.osd_transparent_color_en = 0;
            VO_CORE_Source_Cmd(vo_id,
                GK_VIDEO_SOURCE_SET_OSD, &def_osd_setup);
        } else {
            def_osd_setup.src = GK_OSD_SRC_MAPPED_IN;
            def_osd_setup.osd_direct_mode = 0;
            def_osd_setup.osd_transparent_color = 0;
            def_osd_setup.osd_transparent_color_en = 0;
            def_osd_setup.osd_buf_pitch >>= 1;
            VO_CORE_Source_Cmd(vo_id,
                GK_VIDEO_SOURCE_SET_OSD, &def_osd_setup);

            osd_clut_info.pclut_table = pplatform_info->clut_table;
            osd_clut_info.pblend_table = pplatform_info->blend_table;
            VO_CORE_Source_Cmd(vo_id,
                GK_VIDEO_SOURCE_SET_OSD_CLUT, &osd_clut_info);
        }
        break;

    default:
        break;
    }

    return ret;
}

static inline int _osd_on_csc_change_arch(int vo_id, int csc_en)
{
    int                         errorCode = 0;
    GK_VO_SinkModeT        sink_mode;
    VO_OSD_SinkCfgT*          psink_cfg;
    VO_SRC_CscInfoT  csc_info;

    psink_cfg = &gk_osd_sink_cfg[vo_id];
    if (!psink_cfg->mtx_init) {
        mutex_init(&psink_cfg->mtx);
        psink_cfg->mtx_init = 1;
    }

    mutex_lock(&psink_cfg->mtx);
    if (!psink_cfg->data_valid) {
        vo_warn("%s: Please init vo first!\n", __func__);
        errorCode = -EACCES;
        mutex_unlock(&psink_cfg->mtx);
        return errorCode;
    } else {
        memcpy(&sink_mode, &psink_cfg->sink_mode,
            sizeof(sink_mode));

        csc_info.path = sink_mode.sink_type;
        switch (sink_mode.sink_type)
        {
        case GK_VO_SINK_TYPE_DIGITAL:
            csc_info.path = GK_VIDEO_SOURCE_CSC_DIGITAL;
            if (csc_en)
                csc_info.mode = GK_VIDEO_SOURCE_CSC_YUVSD2RGB;
            else
                csc_info.mode = GK_VIDEO_SOURCE_CSC_RGB2RGB;
            csc_info.clamp =
                GK_VIDEO_SOURCE_CSC_DATARANGE_DIGITAL_SD_FULL;
            break;

        case GK_VO_SINK_TYPE_CVBS:
            csc_info.path = GK_VIDEO_SOURCE_CSC_ANALOG;
            if (csc_en)
                csc_info.mode = GK_VIDEO_SOURCE_CSC_ANALOG_SD;
            else
                csc_info.mode = GK_VIDEO_SOURCE_CSC_RGB2RGB;
            csc_info.clamp = GK_VIDEO_SOURCE_CSC_ANALOG_CLAMP_SD;
            break;

        case GK_VO_SINK_TYPE_HDMI:
            csc_info.path = GK_VIDEO_SOURCE_CSC_HDMI;
            if (csc_en)
                csc_info.mode = GK_VIDEO_SOURCE_CSC_YUVSD2RGB;
            else
                csc_info.mode = GK_VIDEO_SOURCE_CSC_RGB2RGB;
            csc_info.clamp =
                GK_VIDEO_SOURCE_CSC_DATARANGE_DIGITAL_SD_FULL;
            break;

        default:
            break;
        }
        errorCode = VO_CORE_Source_Cmd(vo_id, GK_VIDEO_SOURCE_SET_CSC_DYNAMICALLY, &csc_info);
        if (errorCode)
            return errorCode;

        sink_mode.csc_en = csc_en;

    }
    mutex_unlock(&psink_cfg->mtx);

    errorCode = gk_osd_on_vo_change(vo_id, &sink_mode);

    return errorCode;
}

/*==================Use buffer mapped to user space directly==================*/
static int _direct_pan_display(struct fb_var_screeninfo *var,
    struct fb_info *info)
{
    VO_OSD_InfoT   *posd_info;
    u8              *pSrcOSDBuffer, *pDstOSDBuffer;
    int             errorCode;

    posd_info = &gk_osd_info[info->node];

    mutex_lock(&posd_info->mtx);

    pSrcOSDBuffer = info->screen_base + info->fix.line_length * var->yoffset;
    pDstOSDBuffer = pSrcOSDBuffer;
    clean_d_cache(pDstOSDBuffer, info->fix.line_length * var->yres);

    errorCode = _direct_pan_display_arch(info, posd_info, pDstOSDBuffer);

    if (posd_info->auto_copy) {
        pSrcOSDBuffer = info->screen_base + info->fix.line_length * var->yoffset;
        if (var->yoffset) {
            pDstOSDBuffer = info->screen_base;
        } else {
            pDstOSDBuffer = info->screen_base + info->fix.line_length * var->yres;
        }
        _fb_memcpy(pDstOSDBuffer, pSrcOSDBuffer, info->fix.line_length * var->yres);
    }

    mutex_unlock(&posd_info->mtx);

    return errorCode;
}

/*===================================rgb2clut=================================*/
static inline int _rgb2clut_init(VO_OSD_InfoT *posd_info, u32 pitch, u32 height)
{
    VO_OSD_Rgb2ClutT   *prgb2clut;
    u32                 index, size, color_depth;
    u32                 r, g, b;
    u8                  **ppLUT, **ppBuffer;
    int                 errorCode = 0;

    prgb2clut = &posd_info->rgb2clut_info;
    ppLUT = (u8 **)&prgb2clut->pLUT;
    ppBuffer = (u8 **)&prgb2clut->pOSDBuffer;

    if (*ppLUT) {
        kfree(*ppLUT);
        *ppLUT = NULL;
    }

    if (*ppBuffer) {
        kfree(*ppBuffer);
        *ppBuffer = NULL;
    }

    /* Malloc OSD Buffer */
    if (!posd_info->conversion_buf.available) {
        size = 2 * pitch * height;
        if (size == 0) {
            errorCode = -EINVAL;
            return errorCode;
        }

        *ppBuffer = kzalloc(size, GFP_KERNEL);
        if (!(*ppBuffer)) {
            vo_err("%s: pOSDBuffer out of memory!\n", __func__);
            errorCode = -ENOMEM;
            return errorCode;
        }
    }

    /* Malloc LUT Buffer */
    color_depth = (0x1 << 16);
    *ppLUT = kzalloc(color_depth, GFP_KERNEL);
    if (!(*ppLUT)) {
        vo_err("%s: pLUT out of memory!\n", __func__);
        kfree(*ppBuffer);
        *ppBuffer = NULL;
        errorCode = -ENOMEM;
        return errorCode;
    }

    /* Fill LUT */
    for (index = 0; index < color_depth; index++) {
        r = (index & 0xF800) >> 8;
        g = (index & 0x07E0) >> 3;
        b = (index & 0x001F) << 3;

        (*ppLUT)[index] = (300 * r + 600 * g + 110 * b) >> 10;
        if (!(*ppLUT)[index])
            (*ppLUT)[index] = 1;
    }

    return errorCode;
}

static inline void _convert_rgb2clut(VO_OSD_InfoT *posd_info,
    u16 *pSrc, u8 *pDest, u32 pitch, u32 xres, u32 yres)
{
    u32                 tmp_Src, tmp_Dest;
    u32                 *ptmp_Src, *ptmp_Dest;
    u32                 src_pitch, dest_pitch;
    u32                 i, j, k;
    VO_OSD_Rgb2ClutT   *prgb2clut;

    prgb2clut   = &posd_info->rgb2clut_info;
    ptmp_Src    = (u32 *)pSrc;
    ptmp_Dest   = (u32 *)pDest;
    src_pitch   = pitch >> 2;
    dest_pitch  = pitch >> 3;
    xres >>= 2;

    for (j = 0; j < yres; j++) {
        for (i = 0; i < xres; i++) {
            k = (i << 1) + 1;
            tmp_Src = ptmp_Src[k--];
            tmp_Dest = prgb2clut->pLUT[tmp_Src >> 16];
            tmp_Dest <<= 8;
            tmp_Dest |= prgb2clut->pLUT[tmp_Src & 0xFFFF];
            tmp_Dest <<= 8;
            tmp_Src = ptmp_Src[k];
            tmp_Dest |= prgb2clut->pLUT[tmp_Src >> 16];
            tmp_Dest <<= 8;
            tmp_Dest |= prgb2clut->pLUT[tmp_Src & 0xFFFF];

            ptmp_Dest[i] = tmp_Dest;
        }
        ptmp_Src += src_pitch;
        ptmp_Dest += dest_pitch;
    }
}

static int _rgb2clut_pan_display(struct fb_var_screeninfo *var,
    struct fb_info *info)
{
    int                 errorCode = 0;
    u16                 *pSrcOSDBuffer;
    u8                  *pDestOSDBuffer;
    VO_OSD_InfoT       *posd_info;
    VO_OSD_Rgb2ClutT   *prgb2clut;

    posd_info = &gk_osd_info[info->node];
    prgb2clut = &posd_info->rgb2clut_info;

    mutex_lock(&posd_info->mtx);

    pSrcOSDBuffer = (u16 *)(info->screen_base +
        info->fix.line_length * var->yoffset);


    prgb2clut->buffer_id++;
    prgb2clut->buffer_id &= 0x1;
    if (!posd_info->conversion_buf.available) {
        pDestOSDBuffer = prgb2clut->pOSDBuffer +
            var->yres * prgb2clut->buffer_id * (info->fix.line_length >> 1);
    } else {
        if (!prgb2clut->buffer_id) {
            pDestOSDBuffer = posd_info->conversion_buf.ping_buf;
        } else {
            pDestOSDBuffer = posd_info->conversion_buf.pong_buf;
        }
    }

    _convert_rgb2clut(posd_info, pSrcOSDBuffer,
        pDestOSDBuffer, info->fix.line_length,
        info->var.xres, info->var.yres);

    clean_d_cache(pDestOSDBuffer, info->fix.line_length * var->yres);
    _rgb2clut_pan_display_arch(info, posd_info, pDestOSDBuffer);

    mutex_unlock(&posd_info->mtx);
    return errorCode;
}

/*=============================rgb565_2_uyv565================================*/
static inline int _rgb565_2_uyv565_init(VO_OSD_InfoT *posd_info,
    u32 pitch, u32 yres)
{
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    u16                 **ppLUT;
    u8                  **ppOSDBuffer;
    u32                 index;
    u8                  r, g, b;
    u8                  y, u, v;
    int                 errorCode = 0;

    prgb2yuv    = &posd_info->rgb565_2_uyv565_info;
    ppLUT       = &prgb2yuv->pLUT;
    ppOSDBuffer = (u8 **)&prgb2yuv->pOSDBuffer;

    if (*ppLUT) {
        kfree(*ppLUT);
        *ppLUT = NULL;
    }

    *ppLUT = kmalloc((0x1 << 16) * sizeof(u16), GFP_KERNEL);
    if (!(*ppLUT)) {
        vo_err("%s: pLUT out of memory!\n", __func__);
        errorCode = -ENOMEM;
        return errorCode;
    }

    if (*ppOSDBuffer) {
        kfree(*ppOSDBuffer);
        *ppOSDBuffer = NULL;
    }

    if (!posd_info->conversion_buf.available) {
        *ppOSDBuffer = kzalloc(pitch * yres * 2, GFP_KERNEL);
        if (!(*ppOSDBuffer)) {
            vo_err("%s: pOSDBuffer out of memory(pitch = %d, "
                "yres = %d)!\n", __func__, pitch, yres);
            errorCode = -ENOMEM;
            return errorCode;
        }
    }

    for (index = 0; index < (0x1 << 16); index++) {
        r = (index & 0xF800) >> 8;
        g = (index & 0x07E0) >> 3;
        b = (index & 0x001F) << 3;

        y = (( 66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
        u = ((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
        v = ((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;

        y >>= 2;
        u >>= 3;
        v >>= 3;

        (*ppLUT)[index] = ((u << 11) | (y << 5) | (v));
    }

    return errorCode;
}

static inline void _convert_rgb565_2_uyv565(VO_OSD_InfoT *posd_info,
    u16 *pSrc, u16 *pDst,
    u32 pitch, u32 xres, u32 yres)
{
    u32                 i, size;
    u32                 *prgb, *puyv;
    u32                 rgb32, uyv32;
    u16                 *pLUT;

    prgb = (u32 *)pSrc;
    puyv = (u32 *)pDst;
    pLUT = posd_info->rgb565_2_uyv565_info.pLUT;
    size = (pitch * yres) >> 4;

    for (i = 0; i < size; i++) {
        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;
    }

}

static int _rgb565_2_uyv565_pan_display(struct fb_var_screeninfo *var,
    struct fb_info *info)
{
    VO_OSD_InfoT       *posd_info;
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    int                 errorCode = 0;
    u16                 *pSrcOSDBuffer, *pDstOSDBuffer = NULL;

    posd_info = &gk_osd_info[info->node];
    prgb2yuv = &posd_info->rgb565_2_uyv565_info;

    mutex_lock(&posd_info->mtx);

    prgb2yuv->buffer_id++;
    prgb2yuv->buffer_id &= 0x1;
    pSrcOSDBuffer = (u16 *)(info->screen_base +
        info->fix.line_length * var->yoffset);
    if (!posd_info->conversion_buf.available) {
        pDstOSDBuffer = (u16 *)(prgb2yuv->pOSDBuffer +
            var->yres * prgb2yuv->buffer_id * info->fix.line_length);
    } else {
        if (!prgb2yuv->buffer_id) {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.ping_buf);
        } else {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.pong_buf);
        }
    }

    _convert_rgb565_2_uyv565(posd_info, pSrcOSDBuffer, pDstOSDBuffer,
        info->fix.line_length, info->var.xres, info->var.yres);

    if (!pDstOSDBuffer) {
        errorCode = -1;
        mutex_unlock(&posd_info->mtx);
        return errorCode;
    }

    clean_d_cache(pDstOSDBuffer, info->fix.line_length * var->yres);
    errorCode = _rgb2yuv_pan_display_arch(info, posd_info, pDstOSDBuffer);

    mutex_unlock(&posd_info->mtx);
    return errorCode;
}

/*=============================bgr565_2_uyv565================================*/
static inline int _bgr565_2_uyv565_init(VO_OSD_InfoT *posd_info,
    u32 pitch, u32 yres)
{
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    u16                 **ppLUT;
    u8                  **ppOSDBuffer;
    u32                 index;
    u8                  r, g, b;
    u8                  y, u, v;
    int                 errorCode = 0;

    prgb2yuv    = &posd_info->bgr565_2_uyv565_info;
    ppLUT       = &prgb2yuv->pLUT;
    ppOSDBuffer = (u8 **)&prgb2yuv->pOSDBuffer;

    if (*ppLUT) {
        kfree(*ppLUT);
        *ppLUT = NULL;
    }

    *ppLUT = kmalloc((0x1 << 16) * sizeof(u16), GFP_KERNEL);
    if (!(*ppLUT)) {
        vo_err("%s: pLUT out of memory!\n", __func__);
        errorCode = -ENOMEM;
        return errorCode;
    }

    if (*ppOSDBuffer) {
        kfree(*ppOSDBuffer);
        *ppOSDBuffer = NULL;
    }

    if (!posd_info->conversion_buf.available) {
        *ppOSDBuffer = kzalloc(pitch * yres * 2, GFP_KERNEL);
        if (!(*ppOSDBuffer)) {
            vo_err("%s: pOSDBuffer out of memory(pitch = %d, "
                "yres = %d)!\n", __func__, pitch, yres);
            errorCode = -ENOMEM;
            return errorCode;
        }
    }

    for (index = 0; index < (0x1 << 16); index++) {
        b = (index & 0xF800) >> 8;
        g = (index & 0x07E0) >> 3;
        r = (index & 0x001F) << 3;

        y = (( 66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
        u = ((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
        v = ((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;

        y >>= 2;
        u >>= 3;
        v >>= 3;

        (*ppLUT)[index] = ((u << 11) | (y << 5) | (v));
    }

    return errorCode;
}

static inline void _convert_bgr565_2_uyv565(VO_OSD_InfoT *posd_info,
    u16 *pSrc, u16 *pDst,
    u32 pitch, u32 xres, u32 yres)
{
    u32                 i, size;
    u32                 *prgb, *puyv;
    u32                 rgb32, uyv32;
    u16                 *pLUT;

    prgb = (u32 *)pSrc;
    puyv = (u32 *)pDst;
    pLUT = posd_info->bgr565_2_uyv565_info.pLUT;
    size = (pitch * yres) >> 4;

    for (i = 0; i < size; i++) {
        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;
    }

}

static int _bgr565_2_uyv565_pan_display(struct fb_var_screeninfo *var,
    struct fb_info *info)
{
    VO_OSD_InfoT       *posd_info;
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    int                 errorCode = 0;
    u16                 *pSrcOSDBuffer, *pDstOSDBuffer = NULL;

    posd_info = &gk_osd_info[info->node];
    prgb2yuv = &posd_info->bgr565_2_uyv565_info;

    mutex_lock(&posd_info->mtx);

    prgb2yuv->buffer_id++;
    prgb2yuv->buffer_id &= 0x1;
    pSrcOSDBuffer = (u16 *)(info->screen_base +
        info->fix.line_length * var->yoffset);
    if (!posd_info->conversion_buf.available) {
        pDstOSDBuffer = (u16 *)(prgb2yuv->pOSDBuffer +
            var->yres * prgb2yuv->buffer_id * info->fix.line_length);
    } else {
        if (!prgb2yuv->buffer_id) {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.ping_buf);
        } else {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.pong_buf);
        }
    }

    _convert_bgr565_2_uyv565(posd_info, pSrcOSDBuffer, pDstOSDBuffer,
        info->fix.line_length, info->var.xres, info->var.yres);

    if (!pDstOSDBuffer) {
        errorCode = -1;
        mutex_unlock(&posd_info->mtx);
        return errorCode;
    }

    clean_d_cache(pDstOSDBuffer, info->fix.line_length * var->yres);
    errorCode = _rgb2yuv_pan_display_arch(info, posd_info, pDstOSDBuffer);

    mutex_unlock(&posd_info->mtx);
    return errorCode;
}


/*===========================argb4444_2_ayuv4444==============================*/
static int _argb4444_2_ayuv4444_init(VO_OSD_InfoT *posd_info,
    u32 pitch, u32 yres)
{
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    u16                 **ppLUT;
    u8                  **ppOSDBuffer;
    u32                 index;
    u8                  a, r, g, b;
    u8                  y, u, v;
    int                 errorCode = 0;

    prgb2yuv    = &posd_info->argb4444_2_ayuv4444_info;
    ppLUT       = &prgb2yuv->pLUT;
    ppOSDBuffer = (u8 **)&prgb2yuv->pOSDBuffer;

    if (*ppLUT) {
        kfree(*ppLUT);
        *ppLUT = NULL;
    }

    *ppLUT = kmalloc((0x1 << 16) * sizeof(u16), GFP_KERNEL);
    if (!(*ppLUT)) {
        vo_err("%s: pLUT out of memory!\n", __func__);
        errorCode = -ENOMEM;
        return errorCode;
    }

    if (*ppOSDBuffer) {
        kfree(*ppOSDBuffer);
        *ppOSDBuffer = NULL;
    }

    if (!posd_info->conversion_buf.available) {
        *ppOSDBuffer = kzalloc(pitch * yres * 2, GFP_KERNEL);
        if (!(*ppOSDBuffer)) {
            vo_err("%s: pOSDBuffer out of memory(pitch = %d, "
                "yres = %d)!\n", __func__, pitch, yres);
            errorCode = -ENOMEM;
            return errorCode;
        }
    }

    for (index = 0; index < (0x1 << 16); index++) {
        a = (index & 0xF000) >> 12;
        r = (index & 0x0F00) >> 4;
        g = (index & 0x00F0) >> 0;
        b = (index & 0x000F) << 4;

        y = (( 66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
        u = ((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
        v = ((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;

        y >>= 4;
        u >>= 4;
        v >>= 4;

        (*ppLUT)[index] = ((a << 12) | (y << 8) | (u << 4) | (v << 0));
    }

    return errorCode;
}

static void _convert_argb4444_2_ayuv4444(VO_OSD_InfoT *posd_info,
    u16 *pSrc, u16 *pDst,
    u32 pitch, u32 xres, u32 yres)
{
    u32                i, size;
    u32                *prgb, *puyv;
    u32                rgb32, uyv32;
    u16                *pLUT;
    prgb = (u32 *)pSrc;
    puyv = (u32 *)pDst;
    pLUT = posd_info->argb4444_2_ayuv4444_info.pLUT;
    size = (pitch * yres) >> 4;
    switch(posd_info->osd_flip) {
       case GK_VO_FLIP_HV: 
        puyv = puyv +  (size << 2) - 1;
        for (i = 0; i < size; i++) {
            rgb32 = *prgb++;
            uyv32 = pLUT[rgb32 >> 16];//swap the most significant byte with the least significant byte
            uyv32 |= pLUT[rgb32 & 0x0ffff] << 16;
            *puyv-- = uyv32;

            rgb32 = *prgb++;
            uyv32 = pLUT[rgb32 >> 16];
            uyv32 |= pLUT[rgb32 & 0x0ffff] << 16;
            *puyv-- = uyv32;

            rgb32 = *prgb++;
            uyv32 = pLUT[rgb32 >> 16];
            uyv32 |= pLUT[rgb32 & 0x0ffff] << 16;
            *puyv-- = uyv32;

            rgb32 = *prgb++;
            uyv32 = pLUT[rgb32 >> 16];
            uyv32 |= pLUT[rgb32 & 0x0ffff] << 16;
            *puyv-- = uyv32;
        }
        break;
        
        default:
        for (i = 0; i < size; i++) {
            rgb32 = *prgb++;
            uyv32 = pLUT[rgb32 >> 16] << 16;
            uyv32 |= pLUT[(rgb32 << 16) >> 16];
            *puyv++ = uyv32;

            rgb32 = *prgb++;
            uyv32 = pLUT[rgb32 >> 16] << 16;
            uyv32 |= pLUT[(rgb32 << 16) >> 16];
            *puyv++ = uyv32;

            rgb32 = *prgb++;
            uyv32 = pLUT[rgb32 >> 16] << 16;
            uyv32 |= pLUT[(rgb32 << 16) >> 16];
            *puyv++ = uyv32;

            rgb32 = *prgb++;
            uyv32 = pLUT[rgb32 >> 16] << 16;
            uyv32 |= pLUT[(rgb32 << 16) >> 16];
            *puyv++ = uyv32;
        }
        break;    
    }

}

static int _argb4444_2_ayuv4444_pan_display(struct fb_var_screeninfo *var,
    struct fb_info *info)
{
    VO_OSD_InfoT       *posd_info;
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    int                 errorCode = 0;
    u16                 *pSrcOSDBuffer, *pDstOSDBuffer = NULL;

    posd_info   = &gk_osd_info[info->node];
    prgb2yuv    = &posd_info->argb4444_2_ayuv4444_info;

    mutex_lock(&posd_info->mtx);

    prgb2yuv->buffer_id++;
    prgb2yuv->buffer_id &= 0x1;
    pSrcOSDBuffer = (u16 *)(info->screen_base +
        info->fix.line_length * var->yoffset);
    if (!posd_info->conversion_buf.available) {
        pDstOSDBuffer = (u16 *)(prgb2yuv->pOSDBuffer +
            var->yres * prgb2yuv->buffer_id * info->fix.line_length);
    } else {
        if (!prgb2yuv->buffer_id) {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.ping_buf);
        } else {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.pong_buf);
        }
    }

    _convert_argb4444_2_ayuv4444(posd_info, pSrcOSDBuffer, pDstOSDBuffer,
        info->fix.line_length, info->var.xres, info->var.yres);

    if (!pDstOSDBuffer) {
        errorCode = -1;
        mutex_unlock(&posd_info->mtx);
        return errorCode;
    }

    clean_d_cache(pDstOSDBuffer, info->fix.line_length * var->yres);
    errorCode = _rgb2yuv_pan_display_arch(info, posd_info, pDstOSDBuffer);

    mutex_unlock(&posd_info->mtx);
    return errorCode;
}


/*===========================rgba4444_2_ayuv4444==============================*/
static int _rgba4444_2_ayuv4444_init(VO_OSD_InfoT *posd_info,
    u32 pitch, u32 yres)
{
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    u16                 **ppLUT;
    u8                  **ppOSDBuffer;
    u32                 index;
    u8                  a, r, g, b;
    u8                  y, u, v;
    int                 errorCode = 0;

    prgb2yuv    = &posd_info->rgba4444_2_ayuv4444_info;
    ppLUT       = &prgb2yuv->pLUT;
    ppOSDBuffer = (u8 **)&prgb2yuv->pOSDBuffer;

    if (*ppLUT) {
        kfree(*ppLUT);
        *ppLUT = NULL;
    }

    *ppLUT = kmalloc((0x1 << 16) * sizeof(u16), GFP_KERNEL);
    if (!(*ppLUT)) {
        vo_err("%s: pLUT out of memory!\n", __func__);
        errorCode = -ENOMEM;
        return errorCode;
    }

    if (*ppOSDBuffer) {
        kfree(*ppOSDBuffer);
        *ppOSDBuffer = NULL;
    }

    if (!posd_info->conversion_buf.available) {
        *ppOSDBuffer = kzalloc(pitch * yres * 2, GFP_KERNEL);
        if (!(*ppOSDBuffer)) {
            vo_err("%s: pOSDBuffer out of memory(pitch = %d, "
                "yres = %d)!\n", __func__, pitch, yres);
            errorCode = -ENOMEM;
            return errorCode;
        }
    }

    for (index = 0; index < (0x1 << 16); index++) {
        r = (index & 0xF000) >> 8;
        g = (index & 0x0F00) >> 4;
        b = (index & 0x00F0) >> 0;
        a = (index & 0x000F) >> 0;

        y = (( 66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
        u = ((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
        v = ((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;

        y >>= 4;
        u >>= 4;
        v >>= 4;

        (*ppLUT)[index] = ((a << 12) | (y << 8) | (u << 4) | (v << 0));
    }

    return errorCode;
}

static void _convert_rgba4444_2_ayuv4444(VO_OSD_InfoT *posd_info,
    u16 *pSrc, u16 *pDst,
    u32 pitch, u32 xres, u32 yres)
{
    u32                 i, size;
    u32                 *prgb, *puyv;
    u32                 rgb32, uyv32;
    u16                 *pLUT;

    prgb = (u32 *)pSrc;
    puyv = (u32 *)pDst;
    pLUT = posd_info->rgba4444_2_ayuv4444_info.pLUT;
    size = (pitch * yres) >> 4;

    for (i = 0; i < size; i++) {
        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        uyv32 = pLUT[rgb32 >> 16] << 16;
        uyv32 |= pLUT[(rgb32 << 16) >> 16];
        *puyv++ = uyv32;
    }
}

static int _rgba4444_2_ayuv4444_pan_display(struct fb_var_screeninfo *var,
    struct fb_info *info)
{
    VO_OSD_InfoT       *posd_info;
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    int                 errorCode = 0;
    u16                 *pSrcOSDBuffer, *pDstOSDBuffer = NULL;

    posd_info   = &gk_osd_info[info->node];
    prgb2yuv    = &posd_info->rgba4444_2_ayuv4444_info;

    mutex_lock(&posd_info->mtx);

    prgb2yuv->buffer_id++;
    prgb2yuv->buffer_id &= 0x1;
    pSrcOSDBuffer = (u16 *)(info->screen_base +
        info->fix.line_length * var->yoffset);
    if (!posd_info->conversion_buf.available) {
        pDstOSDBuffer = (u16 *)(prgb2yuv->pOSDBuffer +
            var->yres * prgb2yuv->buffer_id * info->fix.line_length);
    } else {
        if (!prgb2yuv->buffer_id) {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.ping_buf);
        } else {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.pong_buf);
        }
    }

    _convert_rgba4444_2_ayuv4444(posd_info, pSrcOSDBuffer, pDstOSDBuffer,
        info->fix.line_length, info->var.xres, info->var.yres);

    if (!pDstOSDBuffer) {
        errorCode = -1;
        mutex_unlock(&posd_info->mtx);
        return errorCode;
    }

    clean_d_cache(pDstOSDBuffer, info->fix.line_length * var->yres);
    errorCode = _rgb2yuv_pan_display_arch(info, posd_info, pDstOSDBuffer);

    mutex_unlock(&posd_info->mtx);
    return errorCode;
}

/*===========================rgba8888_2_ayuv4444==============================*/
static int _rgba8888_2_ayuv4444_init(VO_OSD_InfoT *posd_info,
    u32 pitch, u32 yres)
{
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    u16                 **ppLUT;
    u8                  **ppOSDBuffer;
    u32                 index;
    u8                  a, r, g, b;
    u8                  y, u, v;
    int                 errorCode = 0;

    prgb2yuv    = &posd_info->rgba8888_2_ayuv4444_info;
    ppLUT       = &prgb2yuv->pLUT;
    ppOSDBuffer = (u8 **)&prgb2yuv->pOSDBuffer;

    if (*ppLUT) {
        kfree(*ppLUT);
        *ppLUT = NULL;
    }

    *ppLUT = kmalloc((0x1 << 16) * sizeof(u16), GFP_KERNEL);
    if (!(*ppLUT)) {
        vo_err("%s: pLUT out of memory!\n", __func__);
        errorCode = -ENOMEM;
        return errorCode;
    }

    if (*ppOSDBuffer) {
        kfree(*ppOSDBuffer);
        *ppOSDBuffer = NULL;
    }

    if (!posd_info->conversion_buf.available) {
        *ppOSDBuffer = kzalloc(pitch * yres * 2/2, GFP_KERNEL);
        if (!(*ppOSDBuffer)) {
            vo_err("%s: pOSDBuffer out of memory(pitch = %d, "
                "yres = %d)!\n", __func__, pitch, yres);
            errorCode = -ENOMEM;
            return errorCode;
        }
    }

    for (index = 0; index < (0x1 << 16); index++) {
        r = (index & 0xF000) >> 8;
        g = (index & 0x0F00) >> 4;
        b = (index & 0x00F0) >> 0;
        a = (index & 0x000F) >> 0;

        y = (( 66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
        u = ((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
        v = ((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;

        y >>= 4;
        u >>= 4;
        v >>= 4;

        (*ppLUT)[index] = ((a << 12) | (y << 8) | (u << 4) | (v << 0));
    }

    return errorCode;
}

static void _convert_rgba8888_2_ayuv4444(VO_OSD_InfoT *posd_info,
    u16 *pSrc, u16 *pDst,
    u32 pitch, u32 xres, u32 yres)
{
    u32                 i, size;
    u32                 *prgb, *puyv;
    u32                 rgb32, uyv32;
    u16                 rgb16, *pLUT;

    prgb = (u32 *)pSrc;
    puyv = (u32 *)pDst;
    pLUT = posd_info->rgba8888_2_ayuv4444_info.pLUT;
    size = (pitch * yres) >> 5;

    for (i = 0; i < size; i++) {
        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 = pLUT[rgb16] << 16;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 |= pLUT[rgb16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 = pLUT[rgb16] << 16;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 |= pLUT[rgb16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 = pLUT[rgb16] << 16;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 |= pLUT[rgb16];
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 = pLUT[rgb16] << 16;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 |= pLUT[rgb16];
        *puyv++ = uyv32;
    }

}

static int _rgba8888_2_ayuv4444_pan_display(struct fb_var_screeninfo *var,
    struct fb_info *info)
{
    VO_OSD_InfoT       *posd_info;
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    int                 errorCode = 0;
    u16                 *pSrcOSDBuffer, *pDstOSDBuffer = NULL;

    posd_info   = &gk_osd_info[info->node];
    prgb2yuv    = &posd_info->rgba8888_2_ayuv4444_info;

    mutex_lock(&posd_info->mtx);

    prgb2yuv->buffer_id++;
    prgb2yuv->buffer_id &= 0x1;
    pSrcOSDBuffer = (u16 *)(info->screen_base +
        info->fix.line_length * var->yoffset);
    if (!posd_info->conversion_buf.available) {
        pDstOSDBuffer = (u16 *)(prgb2yuv->pOSDBuffer +
            var->yres * prgb2yuv->buffer_id * info->fix.line_length/2);
    } else {
        if (!prgb2yuv->buffer_id) {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.ping_buf);
        } else {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.pong_buf);
        }
    }

    _convert_rgba8888_2_ayuv4444(posd_info, pSrcOSDBuffer, pDstOSDBuffer,
        info->fix.line_length, info->var.xres, info->var.yres);

    if (!pDstOSDBuffer) {
        errorCode = -1;
        mutex_unlock(&posd_info->mtx);
        return errorCode;
    }

    clean_d_cache(pDstOSDBuffer, info->fix.line_length * var->yres);
    errorCode = _rgb2yuv_pan_display_arch(info, posd_info, pDstOSDBuffer);

    mutex_unlock(&posd_info->mtx);
    return errorCode;
}

/*===========================abgr8888_2_ayuv4444==============================*/
static int _abgr8888_2_ayuv4444_init(VO_OSD_InfoT *posd_info,
    u32 pitch, u32 yres)
{
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    u16                 **ppLUT;
    u8                  **ppOSDBuffer;
    u32                 index;
    u8                  a, r, g, b;
    u8                  y, u, v;
    int                 errorCode = 0;

    prgb2yuv    = &posd_info->abgr8888_2_ayuv4444_info;
    ppLUT       = &prgb2yuv->pLUT;
    ppOSDBuffer = (u8 **)&prgb2yuv->pOSDBuffer;

    if (*ppLUT) {
        kfree(*ppLUT);
        *ppLUT = NULL;
    }

    *ppLUT = kmalloc((0x1 << 16) * sizeof(u16), GFP_KERNEL);
    if (!(*ppLUT)) {
        vo_err("%s: pLUT out of memory!\n", __func__);
        errorCode = -ENOMEM;
        return errorCode;
    }

    if (*ppOSDBuffer) {
        kfree(*ppOSDBuffer);
        *ppOSDBuffer = NULL;
    }

    if (!posd_info->conversion_buf.available) {    
        *ppOSDBuffer = kzalloc(pitch * yres * 2/2, GFP_KERNEL);
        if (!(*ppOSDBuffer)) {
            vo_err("%s: pOSDBuffer out of memory(pitch = %d, "
                "yres = %d)!\n", __func__, pitch, yres);
            errorCode = -ENOMEM;
            return errorCode;
        }
    }

    for (index = 0; index < (0x1 << 16); index++) {
        a = (index & 0xF000) >> 12;
        b = (index & 0x0F00) >> 4;
        g = (index & 0x00F0) >> 0;
        r = (index & 0x000F) << 4;

        y = (( 66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
        u = ((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
        v = ((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;

        y >>= 4;
        u >>= 4;
        v >>= 4;

        (*ppLUT)[index] = ((a << 12) | (y << 8) | (u << 4) | (v << 0));
    }

    return errorCode;
}

static void _convert_abgr8888_2_ayuv4444(VO_OSD_InfoT *posd_info,
    u16 *pSrc, u16 *pDst,
    u32 pitch, u32 xres, u32 yres)
{
    u32                 i, size;
    u32                 *prgb, *puyv;
    u32                 rgb32, uyv32;
    u16                 rgb16, *pLUT;

    prgb = (u32 *)pSrc;
    puyv = (u32 *)pDst;
    pLUT = posd_info->abgr8888_2_ayuv4444_info.pLUT;
    size = (pitch * yres) >> 5;

    for (i = 0; i < size; i++) {
        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf0000000) >> 16) |
            ((rgb32 & 0x00f00000) >> 12) |
            ((rgb32 & 0x0000f000) >> 8) |
            ((rgb32 & 0x000000f0) >> 4);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;
    }
}

static int _abgr8888_2_ayuv4444_pan_display(struct fb_var_screeninfo *var,
    struct fb_info *info)
{
    VO_OSD_InfoT       *posd_info;
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    int                 errorCode = 0;
    u16                 *pSrcOSDBuffer, *pDstOSDBuffer = NULL;

    posd_info   = &gk_osd_info[info->node];
    prgb2yuv    = &posd_info->abgr8888_2_ayuv4444_info;

    mutex_lock(&posd_info->mtx);

    prgb2yuv->buffer_id++;
    prgb2yuv->buffer_id &= 0x1;
    pSrcOSDBuffer = (u16 *)(info->screen_base +
        info->fix.line_length * var->yoffset);
    if (!posd_info->conversion_buf.available) {
        pDstOSDBuffer = (u16 *)(prgb2yuv->pOSDBuffer +
            var->yres * prgb2yuv->buffer_id * info->fix.line_length/2);
    } else {
        if (!prgb2yuv->buffer_id) {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.ping_buf);
        } else {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.pong_buf);
        }
    }
    _convert_abgr8888_2_ayuv4444(posd_info, pSrcOSDBuffer, pDstOSDBuffer,
        info->fix.line_length, info->var.xres, info->var.yres);

    if (!pDstOSDBuffer) {
        errorCode = -1;
        mutex_unlock(&posd_info->mtx);
        return errorCode;
    }

    clean_d_cache(pDstOSDBuffer, info->fix.line_length * var->yres);
    errorCode = _rgb2yuv_pan_display_arch(info, posd_info, pDstOSDBuffer);

    mutex_unlock(&posd_info->mtx);
    return errorCode;
}

/*===========================rgba8888_2_ayuv1555==============================*/
static int _rgba8888_2_ayuv1555_init(VO_OSD_InfoT *posd_info,
    u32 pitch, u32 yres)
{
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    u16                 **ppLUT;
    u8                  **ppOSDBuffer;
    u32                 index;
    u8                  a, r, g, b;
    u8                  y, u, v;
    int                 errorCode = 0;

    prgb2yuv    = &posd_info->rgba8888_2_ayuv1555_info;
    ppLUT       = &prgb2yuv->pLUT;
    ppOSDBuffer = (u8 **)&prgb2yuv->pOSDBuffer;

    if (*ppLUT) {
        kfree(*ppLUT);
        *ppLUT = NULL;
    }

    *ppLUT = kmalloc((0x1 << 16) * sizeof(u16), GFP_KERNEL);
    if (!(*ppLUT)) {
        vo_err("%s: pLUT out of memory!\n", __func__);
        errorCode = -ENOMEM;
        return errorCode;
    }

    if (*ppOSDBuffer) {
        kfree(*ppOSDBuffer);
        *ppOSDBuffer = NULL;
    }

    if (!posd_info->conversion_buf.available) {    
        *ppOSDBuffer = kzalloc(pitch * yres * 2/2, GFP_KERNEL);
        if (!(*ppOSDBuffer)) {
            vo_err("%s: pOSDBuffer out of memory(pitch = %d, "
                "yres = %d)!\n", __func__, pitch, yres);          
            errorCode = -ENOMEM;
            return errorCode;
        }
    }

    for (index = 0; index < (0x1 << 16); index++) {
        r = (index & 0xF100) >> 8;
        g = (index & 0x07C0) >> 3;
        b = (index & 0x003E) << 2;
        a = (index & 0x0001);
        
        y = (( 66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
        u = ((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
        v = ((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;

        y >>= 3;
        u >>= 3;
        v >>= 3;

        (*ppLUT)[index] = ((a << 15) | (y << 10) | (u << 5) | (v << 0));
    }

    return errorCode;
}

static void _convert_rgba8888_2_ayuv1555(VO_OSD_InfoT *posd_info,
    u16 *pSrc, u16 *pDst,
    u32 pitch, u32 xres, u32 yres)
{
    u32                 i, size;
    u32                 *prgb, *puyv;
    u32                 rgb32, uyv32;
    u16                 rgb16, *pLUT;

    prgb = (u32 *)pSrc;
    puyv = (u32 *)pDst;
    pLUT = posd_info->rgba8888_2_ayuv1555_info.pLUT;
    size = (pitch * yres) >> 5;

    for (i = 0; i < size; i++) {
        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf8000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 13) |
            ((rgb32 & 0x0000f800) >> 10) |
            ((rgb32 & 0x00000080) >> 7);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf8000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 13) |
            ((rgb32 & 0x0000f800) >> 10) |
            ((rgb32 & 0x00000080) >> 7);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf8000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 13) |
            ((rgb32 & 0x0000f800) >> 10) |
            ((rgb32 & 0x00000080) >> 7);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf8000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 13) |
            ((rgb32 & 0x0000f800) >> 10) |
            ((rgb32 & 0x00000080) >> 7);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf8000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 13) |
            ((rgb32 & 0x0000f800) >> 10) |
            ((rgb32 & 0x00000080) >> 7);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf8000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 13) |
            ((rgb32 & 0x0000f800) >> 10) |
            ((rgb32 & 0x00000080) >> 7);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf8000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 13) |
            ((rgb32 & 0x0000f800) >> 10) |
            ((rgb32 & 0x00000080) >> 7);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0xf8000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 13) |
            ((rgb32 & 0x0000f800) >> 10) |
            ((rgb32 & 0x00000080) >> 7);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;
    }
}

static int _rgba8888_2_ayuv1555_pan_display(struct fb_var_screeninfo *var,
    struct fb_info *info)
{
    VO_OSD_InfoT       *posd_info;
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    int                 errorCode = 0;
    u16                 *pSrcOSDBuffer, *pDstOSDBuffer = NULL;

    posd_info   = &gk_osd_info[info->node];
    prgb2yuv    = &posd_info->rgba8888_2_ayuv1555_info;

    mutex_lock(&posd_info->mtx);

    prgb2yuv->buffer_id++;
    prgb2yuv->buffer_id &= 0x1;
    pSrcOSDBuffer = (u16 *)(info->screen_base +
        info->fix.line_length * var->yoffset);
    if (!posd_info->conversion_buf.available) {
        pDstOSDBuffer = (u16 *)(prgb2yuv->pOSDBuffer +
            var->yres * prgb2yuv->buffer_id * info->fix.line_length/2);
    } else {
        if (!prgb2yuv->buffer_id) {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.ping_buf);
        } else {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.pong_buf);
        }
    }
    _convert_rgba8888_2_ayuv1555(posd_info, pSrcOSDBuffer, pDstOSDBuffer,
        info->fix.line_length, info->var.xres, info->var.yres);

    if (!pDstOSDBuffer) {
        errorCode = -1;
        mutex_unlock(&posd_info->mtx);
        return errorCode;
    }

    clean_d_cache(pDstOSDBuffer, info->fix.line_length * var->yres);
    errorCode = _rgb2yuv_pan_display_arch(info, posd_info, pDstOSDBuffer);

    mutex_unlock(&posd_info->mtx);
    return errorCode;
}


/*===========================abgr8888_2_ayuv1555==============================*/
static int _abgr8888_2_ayuv1555_init(VO_OSD_InfoT *posd_info,
    u32 pitch, u32 yres)
{
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    u16                 **ppLUT;
    u8                  **ppOSDBuffer;
    u32                 index;
    u8                  r, g, b;
    u16                 a;
    u8                  y, u, v;
    int                 errorCode = 0;

    prgb2yuv    = &posd_info->abgr8888_2_ayuv1555_info;
    ppLUT       = &prgb2yuv->pLUT;
    ppOSDBuffer = (u8 **)&prgb2yuv->pOSDBuffer;

    if (*ppLUT) {
        kfree(*ppLUT);
        *ppLUT = NULL;
    }

    *ppLUT = kmalloc((0x1 << 16) * sizeof(u16), GFP_KERNEL);
    if (!(*ppLUT)) {
        vo_err("%s: pLUT out of memory!\n", __func__);
        errorCode = -ENOMEM;
        return errorCode;
    }

    if (*ppOSDBuffer) {
        kfree(*ppOSDBuffer);
        *ppOSDBuffer = NULL;
    }
	
	
	if (!posd_info->conversion_buf.available) {    
        *ppOSDBuffer = kzalloc(pitch * yres * 2/2, GFP_KERNEL);
        if (!(*ppOSDBuffer)) {
            vo_err("%s: pOSDBuffer out of memory(pitch = %d, "
                "yres = %d)!\n", __func__, pitch, yres);          
            errorCode = -ENOMEM;
            return errorCode;
        }
    }

    for (index = 0; index < (0x1 << 16); index++) {
        a = (index & 0x8000);
        b = (index & 0x7C00) >> 7;
        g = (index & 0x03E0) >> 2;
        r = (index & 0x001F) << 3;

        y = (( 66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
        u = ((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
        v = ((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;

        y >>= 3;
        u >>= 3;
        v >>= 3;

        (*ppLUT)[index] = (a | (y << 10) | (u << 5) | (v << 0));
    }

    return errorCode;
}

static void _convert_abgr8888_2_ayuv1555(VO_OSD_InfoT *posd_info,
    u16 *pSrc, u16 *pDst,
    u32 pitch, u32 xres, u32 yres)
{
    u32                 i, size;
    u32                 *prgb, *puyv;
    u32                 rgb32, uyv32;
    u16                 rgb16, *pLUT;

    prgb = (u32 *)pSrc;
    puyv = (u32 *)pDst;
    pLUT = posd_info->abgr8888_2_ayuv1555_info.pLUT;
    size = (pitch * yres) >> 5;

    for (i = 0; i < size; i++) {
        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0x80000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 9) |
            ((rgb32 & 0x0000f800) >> 6) |
            ((rgb32 & 0x000000f8) >> 3);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0x80000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 9) |
            ((rgb32 & 0x0000f800) >> 6) |
            ((rgb32 & 0x000000f8) >> 3);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0x80000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 9) |
            ((rgb32 & 0x0000f800) >> 6) |
            ((rgb32 & 0x000000f8) >> 3);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0x80000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 9) |
            ((rgb32 & 0x0000f800) >> 6) |
            ((rgb32 & 0x000000f8) >> 3);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0x80000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 9) |
            ((rgb32 & 0x0000f800) >> 6) |
            ((rgb32 & 0x000000f8) >> 3);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0x80000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 9) |
            ((rgb32 & 0x0000f800) >> 6) |
            ((rgb32 & 0x000000f8) >> 3);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0x80000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 9) |
            ((rgb32 & 0x0000f800) >> 6) |
            ((rgb32 & 0x000000f8) >> 3);
        uyv32 = pLUT[rgb16];

        rgb32 = *prgb++;
        rgb16 = ((rgb32 & 0x80000000) >> 16) |
            ((rgb32 & 0x00f80000) >> 9) |
            ((rgb32 & 0x0000f800) >> 6) |
            ((rgb32 & 0x000000f8) >> 3);
        uyv32 |= (pLUT[rgb16] << 16);
        *puyv++ = uyv32;
    }

}

static int _abgr8888_2_ayuv1555_pan_display(struct fb_var_screeninfo *var,
    struct fb_info *info)
{
    VO_OSD_InfoT       *posd_info;
    VO_OSD_Rgb2YuvT    *prgb2yuv;
    int                 errorCode = 0;
    u16                 *pSrcOSDBuffer, *pDstOSDBuffer = NULL;

    posd_info   = &gk_osd_info[info->node];
    prgb2yuv    = &posd_info->abgr8888_2_ayuv1555_info;

    mutex_lock(&posd_info->mtx);

    prgb2yuv->buffer_id++;
    prgb2yuv->buffer_id &= 0x1;
    pSrcOSDBuffer = (u16 *)(info->screen_base +
        info->fix.line_length * var->yoffset);
    if (!posd_info->conversion_buf.available) {
        pDstOSDBuffer = (u16 *)(prgb2yuv->pOSDBuffer +
            var->yres * prgb2yuv->buffer_id * info->fix.line_length/2);
    } else {
        if (!prgb2yuv->buffer_id) {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.ping_buf);
        } else {
            pDstOSDBuffer = (u16 *)(posd_info->conversion_buf.pong_buf);
        }
    }
    _convert_abgr8888_2_ayuv1555(posd_info, pSrcOSDBuffer, pDstOSDBuffer,
        info->fix.line_length, info->var.xres, info->var.yres);

    if (!pDstOSDBuffer) {
        errorCode = -1;
        mutex_unlock(&posd_info->mtx);
        return errorCode;
    }

    clean_d_cache(pDstOSDBuffer, info->fix.line_length * var->yres);
    errorCode = _rgb2yuv_pan_display_arch(info, posd_info, pDstOSDBuffer);

    mutex_unlock(&posd_info->mtx);
    return errorCode;
}

static int _fb_set_par(struct fb_info *info)
{
    int                     errorCode = 0, i;
    VO_OSD_InfoT           *posd_info;
    GK_VO_SinkModeT    sink_mode;
    VO_OSD_SinkCfgT       *psink_cfg;
    u32                     flag;

    posd_info = &gk_osd_info[info->node];
    for (i = 0; i < 2; i++) {
        if (posd_info->vo_id[i] == -1)
            continue;

        psink_cfg = &gk_osd_sink_cfg[i];
        if (!psink_cfg->mtx_init) {
            mutex_init(&psink_cfg->mtx);
            psink_cfg->mtx_init = 1;
        }

        mutex_lock(&psink_cfg->mtx);
        if (!psink_cfg->data_valid) {
            vo_warn("%s: Please init vo first!\n", __func__);
            errorCode = -EACCES;
            mutex_unlock(&psink_cfg->mtx);
            return errorCode;
        } else {
            memcpy(&sink_mode, &psink_cfg->sink_mode, sizeof(sink_mode));
        }
        mutex_unlock(&psink_cfg->mtx);

        errorCode = gk_osd_on_vo_change(i, &sink_mode);

        flag = GK_VIDEO_SOURCE_UPDATE_OSD_SETUP;
        errorCode = VO_CORE_Source_Cmd(i, GK_VIDEO_SOURCE_UPDATE_VO_SETUP, &flag);
    }

    return errorCode;
}

static int _fb_set_blank(int blank_mode, struct fb_info *info)
{
    int                errorCode = 0;

    switch (blank_mode)
    {
    case FB_BLANK_NORMAL:
    case FB_BLANK_VSYNC_SUSPEND:
    case FB_BLANK_HSYNC_SUSPEND:
    case FB_BLANK_POWERDOWN:
        memset((void *)info->screen_base, 0, info->fix.smem_len);
        break;

    default:
        break;
    }

    return errorCode;
}

static int _fb_set_cmap(struct fb_cmap *cmap, struct fb_info *info)
{
    int                     errorCode = 0, i;
    VO_OSD_InfoT           *posd_info;
    GK_VO_SinkModeT    sink_mode;
    VO_OSD_SinkCfgT       *psink_cfg;
    u32                     flag;

    posd_info = &gk_osd_info[info->node];
    if (posd_info->color_format != GK_FB_COLOR_CLUT_8BPP)
        return errorCode;

    for (i = 0; i < 2; i++) {
        if (posd_info->vo_id[i] == -1)
            continue;

        psink_cfg = &gk_osd_sink_cfg[i];
        if (!psink_cfg->mtx_init) {
            mutex_init(&psink_cfg->mtx);
            psink_cfg->mtx_init = 1;
        }

        mutex_lock(&psink_cfg->mtx);
        if (!psink_cfg->data_valid) {
            vo_warn("%s: Please init vo first!\n", __func__);
            errorCode = -EACCES;
            mutex_unlock(&psink_cfg->mtx);
            return errorCode;
        } else {
            memcpy(&sink_mode, &psink_cfg->sink_mode,
                sizeof(sink_mode));
        }
        mutex_unlock(&psink_cfg->mtx);

        posd_info->cmap_changed = 1;
        errorCode = gk_osd_on_vo_change(i, &sink_mode);

        flag = GK_VIDEO_SOURCE_UPDATE_OSD_SETUP;
        errorCode = VO_CORE_Source_Cmd(i,
                GK_VIDEO_SOURCE_UPDATE_VO_SETUP, &flag);
    }

    return errorCode;
}

/* Notice: Ideally, when one fb displays on two vos, both vo should enable
       csc or disable csc; but if not, fb will always use the one that was
       first initialized
*/
int gk_osd_on_vo_change(int vo_id, GK_VO_SinkModeT *sink_mode)
{
    int                         errorCode = 0, fb_id, skip_fb = 0;
    VO_OSD_InfoT               *posd_info;
    struct gk_platform_fb   *pplatform_info;
    struct gk_fb_media_info *pmedia_info;
    VO_OSD_SinkCfgT           *psink_cfg;
    int                         fb_reinit = 0;

    // Malloc Local Memory
    pplatform_info  = kzalloc(sizeof(struct gk_platform_fb), GFP_KERNEL);
    pmedia_info     = kzalloc(sizeof(struct gk_fb_media_info), GFP_KERNEL);
    if (!pplatform_info || !pmedia_info) {
        printk("%s: Out of memory!\n", __func__);
        if (pplatform_info) kfree(pplatform_info);
        if (pmedia_info) kfree(pmedia_info);

        return errorCode;
    }

    // Update sink config
    psink_cfg = &gk_osd_sink_cfg[vo_id];
    if (!psink_cfg->mtx_init) {
        mutex_init(&psink_cfg->mtx);
        psink_cfg->mtx_init = 1;
    }
    mutex_lock(&psink_cfg->mtx);
    memcpy(&psink_cfg->sink_mode, sink_mode, sizeof(*sink_mode));
    psink_cfg->data_valid = 1;
    mutex_unlock(&psink_cfg->mtx);

    // Judge whether vo uses valid fb
    fb_id = sink_mode->fb_id;
    if (fb_id < 0 || fb_id >= NUM_FB)
        skip_fb = 1;
    if (!skip_fb && gk_fb_get_platform_info(fb_id, pplatform_info))
        skip_fb = 1;
    if (!skip_fb && (pplatform_info->fb_status != GK_FB_ACTIVE_MODE))
        skip_fb = 1;
    if (skip_fb) {
        for (fb_id = 0; fb_id < NUM_FB; fb_id++) {
            posd_info = &gk_osd_info[fb_id];
            if (!posd_info->mtx_init) {
                mutex_init(&posd_info->mtx);
                posd_info->mtx_init = 1;
            }
            mutex_lock(&posd_info->mtx);
            posd_info->vo_id[vo_id] = -1;
            mutex_unlock(&posd_info->mtx);
        }
        _osd_on_vo_change_arch(vo_id, sink_mode, NULL, NULL);
        if (pplatform_info) kfree(pplatform_info);
        if (pmedia_info) kfree(pmedia_info);

        return errorCode;
    }

    /* Update fb info */
    posd_info = &gk_osd_info[fb_id];
    if (!posd_info->mtx_init) {
        mutex_init(&posd_info->mtx);
        posd_info->mtx_init = 1;
    }

    mutex_lock(&posd_info->mtx);
    posd_info->vo_id[vo_id] = vo_id;
    posd_info->osd_flip = psink_cfg->sink_mode.osd_flip;
    _osd_on_vo_change_pre_arch(posd_info);
    if (pplatform_info->color_format != posd_info->color_format) {
        posd_info->color_format = pplatform_info->color_format;
        fb_reinit = 1;
    }

    if (pplatform_info->conversion_buf.available != posd_info->conversion_buf.available
        || pplatform_info->conversion_buf.ping_buf != posd_info->conversion_buf.ping_buf
        || pplatform_info->conversion_buf.ping_buf_size != posd_info->conversion_buf.ping_buf_size
        || pplatform_info->conversion_buf.pong_buf != posd_info->conversion_buf.pong_buf
        || pplatform_info->conversion_buf.pong_buf_size != posd_info->conversion_buf.pong_buf_size)
        {
        posd_info->conversion_buf = pplatform_info->conversion_buf;
        fb_reinit = 1;
    }

    if (posd_info->cmap_changed &&
        posd_info->color_format == GK_FB_COLOR_CLUT_8BPP) {
        fb_reinit = 1;
        posd_info->cmap_changed = 0;
    }

    if (pplatform_info->screen_var.xres != posd_info->xres) {
        posd_info->xres = pplatform_info->screen_var.xres;
        fb_reinit = 1;
    }

    if (pplatform_info->screen_var.yres != posd_info->yres) {
        posd_info->yres = pplatform_info->screen_var.yres;
        fb_reinit = 1;
    }

    if (sink_mode->csc_en != posd_info->csc_en) {
        posd_info->csc_en = sink_mode->csc_en;
        fb_reinit = 1;
    }

    if (sink_mode->format == GK_VIDEO_FORMAT_INTERLACE)
        posd_info->interlace[vo_id] = 1;
    else
        posd_info->interlace[vo_id] = 0;

    if (sink_mode->mode == GK_VIDEO_MODE_0720_0480_I_VGAP ||
        sink_mode->mode == GK_VIDEO_MODE_0720_0576_I) {
        posd_info->cvbs_resolution[vo_id] = 1;
    } else {
        posd_info->cvbs_resolution[vo_id] = 0;
    }

    if (sink_mode->osd_tailor & GK_VO_OSD_AUTO_COPY) {
        posd_info->auto_copy = 1;
    }

    // Change Vo OSD Setting
    sink_mode->osd_size.width = pplatform_info->screen_var.xres;
    sink_mode->osd_size.height = pplatform_info->screen_var.yres;
    _osd_on_vo_change_arch(vo_id, sink_mode, posd_info, pplatform_info);

    // Re-init fb
    if (fb_reinit) {
        pmedia_info->screen_var = pplatform_info->screen_var;
        pmedia_info->screen_fix = pplatform_info->screen_fix;
        pmedia_info->dsp_status = GK_DSP_ENCODE_MODE;
        switch (pplatform_info->color_format)
        {
        case GK_FB_COLOR_CLUT_8BPP:
            pmedia_info->pan_display = _direct_pan_display;
            break;

        case GK_FB_COLOR_AUTO:
        case GK_FB_COLOR_RGB565:
            if (posd_info->support_direct_mode) {
                if (posd_info->csc_en && !posd_info->support_mixer_csc &&
                    !(sink_mode->osd_tailor & GK_VO_OSD_NO_CSC)) {
                    if (!_rgb565_2_uyv565_init(posd_info,
                        pmedia_info->screen_fix.line_length,
                        pmedia_info->screen_var.yres)) {

                        pmedia_info->pan_display = _rgb565_2_uyv565_pan_display;
                    }
                } else {
                    pmedia_info->pan_display = _direct_pan_display;
                }
            } else {
                if (!_rgb2clut_init(posd_info,
                    pmedia_info->screen_fix.line_length >> 1,
                    pmedia_info->screen_var.yres_virtual)){
                    pmedia_info->pan_display = _rgb2clut_pan_display;
                }
            }
            break;
            
        case GK_FB_COLOR_BGR565:
            if (posd_info->support_direct_mode) {
                if (posd_info->csc_en && !posd_info->support_mixer_csc &&
                    !(sink_mode->osd_tailor & GK_VO_OSD_NO_CSC)) {
                    if (!_bgr565_2_uyv565_init(posd_info,
                        pmedia_info->screen_fix.line_length,
                        pmedia_info->screen_var.yres)) {

                        pmedia_info->pan_display = _bgr565_2_uyv565_pan_display;
                    }
                } else {
                    pmedia_info->pan_display = _direct_pan_display;
                }
            } else {
                if (!_rgb2clut_init(posd_info,
                    pmedia_info->screen_fix.line_length >> 1,
                    pmedia_info->screen_var.yres_virtual)){
                    pmedia_info->pan_display = _rgb2clut_pan_display;
                }
            }
            break;
            
        case GK_FB_COLOR_ARGB4444:          
            if (posd_info->support_direct_mode) {
                if (posd_info->csc_en && !posd_info->support_mixer_csc) {
                    if (!_argb4444_2_ayuv4444_init(posd_info,
                        pmedia_info->screen_fix.line_length,
                        pmedia_info->screen_var.yres)){
                        pmedia_info->pan_display = _argb4444_2_ayuv4444_pan_display;
                    }
                } else {
                    pmedia_info->pan_display = _direct_pan_display;
                }
            } else {
                if (!_rgb2clut_init(posd_info,
                    pmedia_info->screen_fix.line_length >> 1,
                    pmedia_info->screen_var.yres_virtual)){
                    pmedia_info->pan_display = _rgb2clut_pan_display;
                }
            }
            break;

        case GK_FB_COLOR_RGBA4444:
            if (posd_info->support_direct_mode) {
                if (posd_info->csc_en && !posd_info->support_mixer_csc) {
                    if (!_rgba4444_2_ayuv4444_init(posd_info,
                        pmedia_info->screen_fix.line_length,
                        pmedia_info->screen_var.yres)) {
                        pmedia_info->pan_display = _rgba4444_2_ayuv4444_pan_display;
                    }
                } else {
                    pmedia_info->pan_display = _direct_pan_display;
                }
            } else {
                if (!_rgb2clut_init(posd_info,
                    pmedia_info->screen_fix.line_length >> 1,
                    pmedia_info->screen_var.yres_virtual)) {
                    pmedia_info->pan_display = _rgb2clut_pan_display;
                }
            }
            break;

        case GK_FB_COLOR_RGBA8888:
            if (posd_info->support_direct_mode) {
                if (posd_info->csc_en && !posd_info->support_mixer_csc) {
                    if(pplatform_info->convert_format == GK_OSD_COLOR_AYUV1555) {
                        if (!_rgba8888_2_ayuv1555_init(posd_info,
                            pmedia_info->screen_fix.line_length,
                            pmedia_info->screen_var.yres)) {
                            pmedia_info->pan_display = _rgba8888_2_ayuv1555_pan_display;
                        }
                    } else {
                        if (!_rgba8888_2_ayuv4444_init(posd_info,
                            pmedia_info->screen_fix.line_length,
                            pmedia_info->screen_var.yres)) {
                            pmedia_info->pan_display = _rgba8888_2_ayuv4444_pan_display;
                        }
                    }
                } else {
                    pmedia_info->pan_display = _direct_pan_display;
                }
            } else {
                if (!_rgb2clut_init(posd_info,
                    pmedia_info->screen_fix.line_length >> 1,
                    pmedia_info->screen_var.yres_virtual)){
                    pmedia_info->pan_display = _rgb2clut_pan_display;
                }
            }
            break;

        case GK_FB_COLOR_ABGR8888:
            if (posd_info->support_direct_mode) {
                if (posd_info->csc_en && !posd_info->support_mixer_csc) {
                    if(pplatform_info->convert_format == GK_OSD_COLOR_AYUV1555) {
                        if (!_abgr8888_2_ayuv1555_init(posd_info,
                            pmedia_info->screen_fix.line_length,
                            pmedia_info->screen_var.yres)) {
                            pmedia_info->pan_display = _abgr8888_2_ayuv1555_pan_display;
                        }
                    } else {
                        if (!_abgr8888_2_ayuv4444_init(posd_info,
                            pmedia_info->screen_fix.line_length,
                            pmedia_info->screen_var.yres)) {
                            pmedia_info->pan_display = _abgr8888_2_ayuv4444_pan_display;
                        }
                    }
                } else {
                    pmedia_info->pan_display = _direct_pan_display;
                }
            } else {
                if (!_rgb2clut_init(posd_info,
                    pmedia_info->screen_fix.line_length >> 1,
                    pmedia_info->screen_var.yres_virtual)) {
                    pmedia_info->pan_display = _rgb2clut_pan_display;
                }
            }
            break;

        case GK_FB_COLOR_ARGB8888:
            pmedia_info->pan_display = _direct_pan_display;
            break;

        default:
            pmedia_info->pan_display = _direct_pan_display;
            break;
        }
        pmedia_info->set_par = _fb_set_par;
        pmedia_info->setcmap = _fb_set_cmap;
        pmedia_info->set_blank = _fb_set_blank;
        gk_fb_set_media_info(fb_id, pmedia_info);
    }
    mutex_unlock(&posd_info->mtx);

    posd_info = &gk_osd_info[1 - fb_id];
    if (!posd_info->mtx_init) {
        mutex_init(&posd_info->mtx);
        posd_info->mtx_init = 1;
    }

    mutex_lock(&posd_info->mtx);
    posd_info->vo_id[vo_id] = -1;
    mutex_unlock(&posd_info->mtx);

    if (pplatform_info) kfree(pplatform_info);
    if (pmedia_info) kfree(pmedia_info);

    return errorCode;
};

int gk_osd_on_fb_switch(int vo_id, int fb_id)
{
    int                     errorCode = 0;
    GK_VO_SinkModeT    sink_mode;
    VO_OSD_SinkCfgT       *psink_cfg;

    psink_cfg = &gk_osd_sink_cfg[vo_id];
    if (!psink_cfg->mtx_init) {
        mutex_init(&psink_cfg->mtx);
        psink_cfg->mtx_init = 1;
    }

    mutex_lock(&psink_cfg->mtx);
    if (!psink_cfg->data_valid) {
        vo_warn("%s: Please init vo first!\n", __func__);
        errorCode = -EACCES;
        mutex_unlock(&psink_cfg->mtx);
        return errorCode;
    } else {
        memcpy(&sink_mode, &psink_cfg->sink_mode,
            sizeof(sink_mode));
        sink_mode.fb_id = fb_id;
    }

    mutex_unlock(&psink_cfg->mtx);

    errorCode = gk_osd_on_vo_change(vo_id, &sink_mode);

    return errorCode;
}

int gk_osd_on_csc_change(int vo_id, int csc_en)
{
    return _osd_on_csc_change_arch(vo_id, csc_en);
}

int gk_osd_on_rescaler_change(int vo_id, int enable, int width, int height)
{
    int                     errorCode = 0;
    GK_VO_SinkModeT    sink_mode;
    VO_OSD_SinkCfgT       *psink_cfg;

    psink_cfg = &gk_osd_sink_cfg[vo_id];
    if (!psink_cfg->mtx_init) {
        mutex_init(&psink_cfg->mtx);
        psink_cfg->mtx_init = 1;
    }

    mutex_lock(&psink_cfg->mtx);
    if (!psink_cfg->data_valid) {
        vo_warn("%s: Please init vo first!\n", __func__);
        errorCode = -EACCES;
        mutex_unlock(&psink_cfg->mtx);
        return errorCode;
    } else {
        memcpy(&sink_mode, &psink_cfg->sink_mode,
            sizeof(sink_mode));
        if (sink_mode.fb_id < 0) {
            vo_warn("%s: No fb available for vo%d!\n",
                __func__, vo_id);
            errorCode = -EACCES;
            mutex_unlock(&psink_cfg->mtx);
            return errorCode;
        }
        sink_mode.osd_rescale.enable = enable;
        sink_mode.osd_rescale.width = width;
        sink_mode.osd_rescale.height = height;
    }
    mutex_unlock(&psink_cfg->mtx);

    errorCode = gk_osd_on_vo_change(vo_id, &sink_mode);

    return errorCode;
}

int gk_osd_on_offset_change(int vo_id, int change, int x, int y)
{
    int                errorCode = 0;
    GK_VO_SinkModeT    sink_mode;
    VO_OSD_SinkCfgT        *psink_cfg;

    psink_cfg = &gk_osd_sink_cfg[vo_id];
    if (!psink_cfg->mtx_init) {
        mutex_init(&psink_cfg->mtx);
        psink_cfg->mtx_init = 1;
    }

    mutex_lock(&psink_cfg->mtx);
    if (!psink_cfg->data_valid) {
        vo_warn("%s: Please init vo first!\n", __func__);
        errorCode = -EACCES;
        mutex_unlock(&psink_cfg->mtx);
        return errorCode;
    } else {
        memcpy(&sink_mode, &psink_cfg->sink_mode,
            sizeof(sink_mode));
        if (sink_mode.fb_id < 0) {
            vo_warn("%s: No fb available for vo%d!\n",
                __func__, vo_id);
            errorCode = -EACCES;
            mutex_unlock(&psink_cfg->mtx);
            return errorCode;
        }
        sink_mode.osd_offset.specified = change;
        sink_mode.osd_offset.offset_x = x;
        sink_mode.osd_offset.offset_y = y;
    }
    mutex_unlock(&psink_cfg->mtx);

    errorCode = gk_osd_on_vo_change(vo_id, &sink_mode);

    return errorCode;
}

