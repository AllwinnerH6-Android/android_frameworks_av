/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxMovParser.c
 * Description : Mov parser.
 * History :
 *
 */
#define LOG_TAG "CdxMovCrypto"

#include <mediadrm/IMediaDrmService.h>
#include <binder/IServiceManager.h>
#include <media/stagefright/MediaErrors.h>
#include <binder/MemoryDealer.h>
#include <media/ICrypto.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/AUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

using namespace android;
static const uint8_t playready_uuid[17] = {
    0x9a, 0x04, 0xf0, 0x79, 0x98, 0x40, 0x42, 0x86,
    0xab, 0x92, 0xe6, 0x5b, 0xe0, 0x88, 0x5f, 0x95,
    '\0'
};

static struct DrmInfoS {
    sp<MemoryDealer> mDealer;
    sp<IMemory> sourceBase;
    sp<IMemory> dstBase;
    sp<ICrypto> mCrypto;
    int32_t mHeapSeqNum;
} drminfo = { 0,0,0,0,0};


int CreatePlayReadyCryptoPlugin(){
    drminfo.mDealer = new MemoryDealer(512*1024, "CdxMovParser");
    sp<IServiceManager> sm = defaultServiceManager();

    sp<IBinder> binder = sm->getService(String16("media.drm"));
    sp<IMediaDrmService> service = interface_cast<IMediaDrmService>(binder);
    if (service == NULL) {
        return -1;
    }
    sp<ICrypto> crypto = service->makeCrypto();
    if (crypto == NULL || (crypto->initCheck() != OK && crypto->initCheck() != NO_INIT)) {
        return -1;
    }
    status_t err = crypto->createPlugin(playready_uuid,NULL,0);
    if (err != OK) {
        ALOGD("create plugin failed!!");
        return -1;
    }
    drminfo.mCrypto = crypto;
    //this size should change if data large than 128k
    //dst memory should allocate before source memory.(it is up to cryptoHal.cpp)
    drminfo.dstBase = drminfo.mDealer->allocate(128*1024);
    drminfo.sourceBase = drminfo.mDealer->allocate(128*1024);
    if(drminfo.sourceBase == NULL || drminfo.dstBase == NULL){
        ALOGE("allocate buffer failed!!!");
        return -1;
    }
    int32_t seqNum = crypto->setHeap(drminfo.mDealer->getMemoryHeap());
    if (seqNum >= 0) {
        drminfo.mHeapSeqNum = seqNum;
        ALOGD("setHeap returned mHeapSeqNum=%d", drminfo.mHeapSeqNum);
    } else {
        drminfo.mHeapSeqNum = -1;
        ALOGE("setHeap failed, setting mHeapSeqNum=-1");
    }
    return 0;
}

int ClosePlayReadyCryptoPlugin(){
    if (drminfo.mCrypto != nullptr && drminfo.mDealer != nullptr && drminfo.mHeapSeqNum >= 0)
        drminfo.mCrypto->unsetHeap(drminfo.mHeapSeqNum);
    return 0;
}

int PlayReadyCryptoPluginCrypto(uint64_t iv,uint32_t ClearDataNum,uint32_t EncryptedDataNum,
    uint8_t **opaque_buf){
    ICrypto::DestinationBuffer destination;
    //support playready secure decrypt
    //destination.mType = ICrypto::kDestinationTypeNativeHandle;
    //destination.mHandle = nh ;
    destination.mType = ICrypto::kDestinationTypeSharedMemory;
    destination.mSharedMemory = drminfo.dstBase;
    ICrypto::SourceBuffer source;
    source.mSharedMemory = drminfo.sourceBase;
    source.mHeapSeqNum = drminfo.mHeapSeqNum;
    CryptoPlugin::Mode mode = CryptoPlugin::kMode_AES_CTR;
    CryptoPlugin::SubSample ss;
    CryptoPlugin::Pattern pattern;
    ss.mNumBytesOfClearData = ClearDataNum;
    ss.mNumBytesOfEncryptedData = EncryptedDataNum;
    pattern.mEncryptBlocks = 0;
    pattern.mSkipBlocks = 0;
    AString errorDetailMsg;
    //uint8_t *buf1 = (uint8_t *)buf;
    //ALOGV("source[%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x]",
    //buf1[0],buf1[1],buf1[2],buf1[3],
    //buf1[4],buf1[5],buf1[6],buf1[7],
    //buf1[8],buf1[9],buf1[10],buf1[11],
    //buf1[12],buf1[13],buf1[14],buf1[15]);
    ssize_t result = drminfo.mCrypto->decrypt(NULL, (uint8_t *)&iv, mode, pattern,
            source, 0,&ss, 1, destination, &errorDetailMsg);
    if(result < 0){
        ALOGE("decrypt failed!!!errorDetailMsg=[%s]",errorDetailMsg.c_str());
        return -1;
    }
    //get out pointer
    //void *opaque_buf = (void *)nh->data[0];
    *opaque_buf = (uint8_t *)destination.mSharedMemory->pointer();
    return result;

}


void* PlayReadyGetInputBuffer(){
    void *buf = static_cast<void *>(drminfo.sourceBase->pointer());
    return buf;
}
#ifdef __cplusplus
}
#endif

