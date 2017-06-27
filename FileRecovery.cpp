#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include <math.h>
using namespace std;


void outPutMkfs(char inPutAdd[20])
{

    char  temp[100]="sudo dd if=/dev/";
    strcat(temp,inPutAdd);
    strcat(temp, " skip=2048 count=1 of=mkfs.vfat ");
    system(temp);
    //system("xxd mkfs.vfat");//tiao shi ming ling
}
void creatVfat(char inPutAdd[20])
{
    char  temp[100]="sudo mkfs.vfat /dev/";
    strcat(temp,inPutAdd);
    strcat(temp, "1");
    system(temp);
}

bool isFat32(int &num1,int& num2)
{
    if(num1==0&&num2==0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int analyzeVbrAndGetNumber(int &startingLABOfThePartition)
{

    unsigned char vbr[512];//char -127~128     unsigned 0~256
    memset(vbr,0,512);
    int rootDirectory;
    int v[512];
    memset(v,0,512);
   // string a[512];
    int i=0;
    FILE *fp;
    fp=fopen("mkfs.vfat","r");
    if(fp==NULL){cout<<"error";}
    while(i<512)
    {
        vbr[i]=getc(fp);
        v[i]=vbr[i]-0;
        i++;
    }
    /**************************************/
    int sectorsPerCluster=v[13];
    int sizeInSectorsOfTheReservedArea=v[15]*256+v[14];
    int numberOfFATs=v[16];
    int sizeOfEachFATInSectors=v[36]+v[37]*256+v[38]*65536+v[39]*16777216 ;
    int clusterWhereRootDirectoryCanBeFound=(v[44]+v[45]*256+v[46]*65536+v[47]*16777216-2)*sectorsPerCluster;
//*****************************************************************//
    if(isFat32(v[22],v[23]))//FAT32
    {
        rootDirectory=startingLABOfThePartition + sizeInSectorsOfTheReservedArea +
                numberOfFATs*sizeOfEachFATInSectors + clusterWhereRootDirectoryCanBeFound;
    } else//FAT16
    {
        rootDirectory=startingLABOfThePartition + sizeInSectorsOfTheReservedArea +
                numberOfFATs*sizeOfEachFATInSectors;
    }
    return rootDirectory;

}
void outPutRootDir(char inPutAdd[20],int &rootDirNum)
{
    char  temp[100]="sudo dd if=/dev/";
    char num[20];
    sprintf(num,"%d",rootDirNum);
    strcat(temp,inPutAdd);
    strcat(temp," skip=");
    strcat(temp,num);
    strcat(temp, "  count=1 of=rootDir.x ");
    system(temp);
  //  system("xxd rootDir.x");//tiao shi
    getchar();
}
void writeToDisk(unsigned char vbr[512],char inPutAdd[20],int &rootDirNum)
{
    FILE *fp ;
    fp=fopen("rootDir.x","w");
    for (int i = 0; i < 512;i++) {
        fprintf(fp,"%c",vbr[i]);
    }
    fclose(fp);


    char temp[100]="sudo dd if=rootDir.x ";
    char num[20];
    sprintf(num,"%d",rootDirNum);

    strcat(temp," count=1 of=/dev/");
    strcat(temp,inPutAdd);
    strcat(temp," seek=");
    strcat(temp,num);
    system(temp);

    printf("File is here!");
}
void analyzeRootDir(int &rootDirNumber,char inPutAdd[20])
{
    unsigned char vbr[512];//char -127~128     unsigned char 0~256
    memset(vbr, 0, 512);
    int rootDirectory;
    int v[512];
    memset(v, 0, 512);
    // string a[512];
    int i = 0;
    FILE *fp = NULL;
    fp = fopen("rootDir.x", "r");
    if (fp == NULL) { cout << "error"; }
    while (i < 512) {
        vbr[i] = getc(fp);
        v[i] = vbr[i] - 0;
        i++;
    }
//************************************
    int num_inLNF=11;
    for (int j = 0; j < 10; ++j)
    {

        if (v[num_inLNF]==15)//isLNF
        {
            int jishu=2;
            if(v[num_inLNF-11]==229)//is DELETE
            {
                vbr[num_inLNF-11]=(unsigned char)'101';
                vbr[num_inLNF+21]=(unsigned char)'101';
            }
            num_inLNF+=32*jishu;
        } else
        {
            if(v[num_inLNF-11]==229)//is DELETE
            {
                vbr[num_inLNF-11]=(unsigned char)'101';
            }
            num_inLNF+=32;
        }
    }

    writeToDisk(vbr,inPutAdd,rootDirNumber);

    /////////////////////////////////
//    int num_inLNF=11;//chu shi ji shu    isLAN
//    int num_bof=0;//begin of per file
//    for (int k = 0; k <10; k++) // du qu shi ge wen jian
//    {
//        string fileName;
//        char t;
//        if(v[num_inLNF]==15)// isLNF
//        {
//            for (int j = 1; j <11 ; j=j+2) //Get The File Name
//            { t=(char)v[num_bof+j];fileName+=t; }
//            for (int j = 14; j <26 ; j=j+2) //Get The File Name
//            { t=(char)v[num_bof+j];fileName+=t; }
//            for (int j = 28; j <32 ; j=j+2) //Get The File Name
//            { t=(char)v[num_bof+j];fileName+=t; }
//            num_inLNF=num_inLNF+howManyBytes(v[num_bof]);
//            num_bof=num_inLNF-11;
//        }
//        cout<<"FileName:"<<fileName<<endl<<endl;
//        /////////////Created Time




    }



void mountIt(char inPutAdd[20])
{
    char temp[100]="sudo mkdir /mnt/";
    strcat(temp,inPutAdd);
    strcat(temp,"1");
    system(temp);

    char temp1[100]="sudo mount /dev/";
    strcat(temp1,inPutAdd);
    strcat(temp1,"1 /mnt/");
    strcat(temp1,inPutAdd);
    strcat(temp1,"1");
    system(temp1);

    //system("lsblk");
}



void unMountIt(char inPutAdd[20])
{
    char t[100] = "umount /dev/";
    strcat(t, inPutAdd);
    system(t);
    char t1[100] = "umount /dev/";
    strcat(t1, inPutAdd);
    strcat(t1, "1");
    system(t1);

}
int main() {
    char inPutAdd[20];
    int startingLABOfThePartition;
    cout << "\nPlease input the address you want analyze.\n"<<"eg." << "sdb" << endl;
    fflush(stdin);//
    cin >> inPutAdd;
    //******************************************************


    //unMountIt(inPutAdd);
    creatVfat(inPutAdd);
    outPutMkfs(inPutAdd);
    cout << "\nPlease input starting LAB of the partition. \n"<<"eg" << " 2048" << endl;
    fflush(stdin);
    cin >> startingLABOfThePartition;

    int rootNumber=analyzeVbrAndGetNumber(startingLABOfThePartition);
    //mountIt(inPutAdd);


    outPutRootDir(inPutAdd,rootNumber);
    analyzeRootDir(rootNumber,inPutAdd);
    cout<<"\n\nThe program will automatically mount the hard disk.\n";
    unMountIt(inPutAdd);
    mountIt(inPutAdd);
    cout<<"\n\nPress the Enter key and end the program.\n";
    getchar();
    return 0;

}
