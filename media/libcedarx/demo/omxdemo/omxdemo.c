#include "cdx_log.h"
#include <stdlib.h>
#include <stdint.h>
#include "OMX_Core.h"
#include "OMX_Video.h"
#include "OMX_Component.h"

typedef struct xxOmxPort{
    OMX_PARAM_PORTDEFINITIONTYPE portDef;
    OMX_BUFFERHEADERTYPE **buffers;
}xxOmxPort;

typedef struct OmxContex {
    OMX_HANDLETYPE * handle;
    xxOmxPort ports[2];
}OmxContex;



#define OMX_INIT_PARAM(param) {                                  \
    memset (&(param), 0, sizeof ((param)));                      \
    (param).nSize = sizeof (param);                              \
    (param).nVersion.s.nVersionMajor = 1;                        \
    (param).nVersion.s.nVersionMinor = 1;                        \
}

static OMX_ERRORTYPE EventHandler (OMX_HANDLETYPE omx_handle,
OMX_PTR app_data,
OMX_EVENTTYPE event, OMX_U32 data_1, OMX_U32 data_2, OMX_PTR event_data)
{
    logd("event handle");
    return 0;
}

static OMX_ERRORTYPE EmptyBufferDone (OMX_HANDLETYPE omx_handle,
OMX_PTR app_data, OMX_BUFFERHEADERTYPE * omx_buffer)
{
    logd("EmptyBufferDone");
    return OMX_ErrorNone;
}


static OMX_ERRORTYPE FillBufferDone (OMX_HANDLETYPE omx_handle,
OMX_PTR app_data, OMX_BUFFERHEADERTYPE * omx_buffer)
{
    logd("FillBufferDone");
    return OMX_ErrorNone;
}

//* callback function
static OMX_CALLBACKTYPE callbacks =
{ EventHandler, EmptyBufferDone, FillBufferDone };

static inline void change_state (OmxContex * c, OMX_STATETYPE state)
{
    OMX_SendCommand (c->omx_handle, OMX_CommandStateSet, state, NULL);
}

static xxOmxPort* omxCompAddPort(OmxContex* c, int index)
{
    if(index > 2)
    {
        loge("index error");
        return NULL;
    }
    xxOmxPort* port = &c->ports[index];
    OMX_GetParameter(c->handle, OMX_IndexParamPortDefinition, &port->portDef);
    if(port->portDef.eDir == OMX_DirInput)
        logd("input port");
    else
        logd("output port");

    return port;
}

static int omx_codec_config(OmxContex* contex, OMX_BOOL isEncoder, OMX_U32 nFrameWidth,
                      OMX_U32 nFrameHeight, OMX_COLOR_FORMATTYPE color_format)
{
    OMX_VIDEO_PARAM_PORTFORMATTYPE format;
    OMX_PARAM_PORTDEFINITIONTYPE param;
    OMX_VIDEO_PORTDEFINITIONTYPE *video_def = &param.format.video;
    if(isEncoder)
    {
        logd("cannot support encoder now");
    }
    else
    {
        format.nPortIndex = OMX_DirOutput;
        OMX_GetParameter(contex->handle, OMX_IndexParamVideoPortFormat, &param);
        format.eCompressionFormat = OMX_VIDEO_CodingUnused;
        format.eColorFormat = color_format;
        OMX_SetParameter(contex->handle, OMX_IndexParamVideoPortFormat, &param);

        param.nPortIndex = OMX_DirInput;
        OMX_GetParameter(contex->handle, OMX_IndexParamPortDefinition, &param);
        param.nBufferSize = nFrameWidth*nFrameHeight*3/2;
        video_def->nFrameWidth = nFrameWidth;
        video_def->nFrameHeight = nFrameHeight;
        video_def->nStride = nFrameWidth;
        OMX_SetParameter(contex->handle, OMX_IndexParamPortDefinition, &param);

        param.nPortIndex = OMX_DirOutput;
        OMX_GetParameter(contex->handle, OMX_IndexParamPortDefinition, &param);
        param.nBufferSize = nFrameWidth*nFrameHeight*3/2;
        video_def->nFrameWidth = nFrameWidth;
        video_def->nFrameHeight = nFrameHeight;
        video_def->nStride = nFrameWidth;
        OMX_SetParameter(contex->handle, OMX_IndexParamPortDefinition, &param);
    }

    return 0;
}

static void omx_port_allocate_buffer(OmxContex* c, xxOmxPort* port)
{
    OMX_PARAM_PORTDEFINITIONTYPE param;
    int number_buffers = port.portDef.nBufferCountActual;
    int i;
    port->buffers = (OMX_BUFFERHEADERTYPE **)malloc(sizeof(OMX_BUFFERHEADERTYPE *)*number_buffers);

    if(port.portDef.eDir == OMX_DirInput)
    {
        param.nPortIndex = OMX_DirInput;
        OMX_GetParameter(c->handle, OMX_IndexParamPortDefinition, &param);
        for(i=0; i<number_buffers; i++)
        {
            OMX_AllocateBuffer(c->handle, &port->buffer[i],
                port.portDef.nPortIndex, NULL, param.nBufferSize);
        }
    }
    else /* output port */
    {
        logd("=== USE_buffer not support");
        //OMX_UseBuffer(c->handle, &port->buffer[i], port.portDef.nPortIndex,
        //    NULL, port.portDef.nBufferSize, pBuffer);
    }

}

int main()
{
    OmxContex *contex = (OmxContex*)malloc(sizeof(OmxContex));
    memset(contex, 0, sizeof(OmxContex));

    OMX_HANDLETYPE * omxHandle = contex->handle;
    OMX_Init();

    OMX_STRING componentName = "OMX.allwinner.video.decoder.avc";
    OMX_STRING role = "video_decoder.avc";
    OMX_GetHandle(omxHandle, componentName, contex, &callbacks);

    OMX_PARAM_COMPONENTROLETYPE param;
    OMX_INIT_PARAM(param);
    OMX_SetParameter(omxHandle, OMX_IndexParamStandardComponentRole, &param);

    OMX_STATETYPE state;
    OMX_GetState(omxHandle, &state);
    if(state != OMX_StateLoaded)
    {
        logd("==== state error");
        return -1;
    }

    OMX_GetParameter(omxHandle, OMX_IndexParamVideoInit, &param);

    omxCompAddPort(contex, 0 /*input*/);
    omxCompAddPort(contex, 1 /*output*/);

    logd("open decoder");


    omx_codec_config(contex, OMX_FALSE, 1280, 720, OMX_COLOR_FormatYUV420Planar);

    change_state(contex, OMX_StateIdle);
    // allocate buffer
    omx_port_allocate_buffer(contex, contex->ports[0]);
    omx_port_allocate_buffer(contex, contex->ports[1]);




    OMX_Deinit();
    return 0;
}
