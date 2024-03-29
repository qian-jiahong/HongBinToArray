/*
将二进制文件转换成 C 语言数组。
qianjiahong 2024.01.23
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned long int u32;

#define APP_NAME "HongBinToArray"
#define VERSION "1.0.0"

#define NEW_LINE "\n"
#define ARRAY_HEAD "const unsigned char romdata_xxx[%lu] = {\n"
#define GENERATED  "// Generated by %s V%s\n"

#define RET_CODE_NO_ERROR (0)          // 无错误
#define RET_CODE_PARA_ERR (-1)         // 参数错误
#define RET_CODE_LOW_RAM (-2)          // 内存不足
#define RET_CODE_SRC_EMPTY (-3)        // 源文件为空
#define RET_CODE_CAN_NOT_OPEN_SRC (-4) // 无法打开源文件
#define RET_CODE_CAN_NOT_OPEN_DST (-5) // 无法打开输出文件

u32 getFileSize(u8 *path)
{
    u32 len = 0;
    FILE *fp = fopen(path, "rb");

    if (fp == NULL)
    {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fclose(fp);

    return len;
}

int readFile(u8 *path, u8 *buf, u32 fileSize)
{
    FILE *fp = fopen(path, "rb");

    if (fp == NULL)
    {
        return -1;
    }

    fread(buf, sizeof(u8), fileSize, fp);
    fclose(fp);

    return 0;
}

int output(u8 *outpath, u8 *buf, u32 fileSize)
{
    FILE *fOutFile = fopen(outpath, "wa+");
    int i, linePosi;
    u8 pBuf[7] = {0};
    u8 pHead[100] = {0};
    u8 generatedStr[100] = {0};

    if (fOutFile == NULL)
    {
        return -1;
    }

    memset(generatedStr, 0, sizeof(generatedStr));
    sprintf(generatedStr, GENERATED, APP_NAME, VERSION);
    fwrite(generatedStr, strlen(generatedStr), 1, fOutFile);

    memset(pHead, 0, sizeof(pHead));
    sprintf(pHead, ARRAY_HEAD, fileSize);
    fwrite(pHead, strlen(pHead), 1, fOutFile);

    linePosi = 0;
    for (i = 0; i < fileSize; i++)
    {
        u8 sep[2] = {' ', 0};

        sprintf(pBuf, "0x%02X", buf[i]);
        if (i < fileSize - 1)
        {
            strcat(pBuf, ",");
        }

        linePosi++;
        if (linePosi >= 16 || i == (fileSize - 1))
        {
            linePosi = 0;
            strcpy(sep, NEW_LINE);
        }
        strcat(pBuf, sep);

        fwrite(pBuf, strlen(pBuf), 1, fOutFile);
    }

    fwrite("};", strlen("};"), 1, fOutFile);
    fwrite(NEW_LINE, strlen(NEW_LINE), 1, fOutFile);

    fclose(fOutFile);
    return 0;
}

int main(int argc, u8 *argv[])
{
    u8 *buf = NULL;
    u8 srcFile[300] = {0};
    u8 dstFile[300] = {0};
    u32 fileSize = 0;

#ifdef WIN32
    // system("chcp 65001 & cls"); // cls 用来清除 chcp 的输出
#endif

    if (argc < 3)
    {
        printf("参数错误. (错误码: %d)\n", RET_CODE_PARA_ERR);
        printf("\n命令说明:\n");
        printf("  %s <二进制文件名> <输出文件名>\n", APP_NAME);
    }

// 用户双击调用, 不自动关闭窗口
#ifdef WIN32
    if (argc == 1)
    {
        system("pause");
    }
#endif

    if (argc < 3)
    {
        return RET_CODE_PARA_ERR;
    }

    strcpy(srcFile, argv[1]);
    strcpy(dstFile, argv[2]);

    // 获取文件的大小
    fileSize = getFileSize(srcFile);
    if (fileSize < 0)
    {
        printf("无法打开源文件: %s\n(错误码: %d)\n", srcFile, RET_CODE_CAN_NOT_OPEN_SRC);
        return RET_CODE_CAN_NOT_OPEN_SRC;
    }
    else if (fileSize == 0)
    {
        printf("源文件为空: %s\n(错误码: %d)\n", srcFile, RET_CODE_SRC_EMPTY);
        return RET_CODE_SRC_EMPTY;
    }

    // 申请内存，用于存放生成的数组
    buf = (u8 *)malloc(sizeof(u8) * fileSize);
    if (buf == NULL)
    {
        printf("内存不足!\n(错误码: %d)\n", RET_CODE_LOW_RAM);
        return RET_CODE_LOW_RAM;
    }

    // 读取文件
    if (readFile(srcFile, buf, fileSize) < 0)
    {
        printf("无法打开源文件: %s\n(错误码: %d)\n", srcFile, RET_CODE_CAN_NOT_OPEN_SRC);
        return RET_CODE_CAN_NOT_OPEN_SRC;
    }

    // 制作头文件
    if (output(dstFile, buf, fileSize) < 0)
    {
        printf("无法打开输出文件: %s\n(错误码: %d)\n", dstFile, RET_CODE_CAN_NOT_OPEN_DST);
        return RET_CODE_CAN_NOT_OPEN_DST;
    }

    printf("源文件: %s, 大小: %lu 字节\n", srcFile, fileSize);
    printf("输出文件: %s\n", dstFile);

    return RET_CODE_NO_ERROR;
}
