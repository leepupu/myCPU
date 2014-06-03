#include "pipeline_reg.h"

void swap(int *a, int *b)
{
    *a = *a+*b;
    *b = *a-*b;
    *a = *a-*b;
}

PipelineReg::PipelineReg(int len)
{
    if(DEBUG)
        printf("len = %u\n", len);
    data = (int**)(new int*[len]);
    ignoreIdx = -1;

    for(int i=0;i<len;i++)
    {
        data[i] = new int[2];
        data[i][0] = 0;
        data[i][1] = 0;
        //printf("sizeof(data): %d", sizeof(int*));
        if(DEBUG)
            printf("data[%d] at %x, values: %d, %d\n", i, data[i], data[i][0], data[i][1]);
    }
    if(DEBUG)
        printf("addredd: %x\n", data);
    csPre = new ControlSignal();
    csNew = new ControlSignal();
    regLen = len;
    pDumpHelper = new DumpHelper();
}

PipelineReg::~PipelineReg()
{
    if(DEBUG)
        printf("call ~PipelineReg()\n");
    for(int i=0;i<regLen;i++)
        delete[] data[i];
    delete[] data;
    delete csPre;
    delete csNew;
}

void PipelineReg::update()
{
    for(int i=0;i<regLen;i++)
    {
        *((*(data+i))+1) = *(*(data+i));
        *(*(data+i)) = 0;
    }
        //swap((*(data+i)), (*(data+i))+1);
    csPre->copy(csNew);
}

int PipelineReg::getPre(int type)
{
    if(DEBUG)
        printf("getPre(%u) = %u\n", type, (*(data+type))[1]);
    return (*(data+type))[1];
}

int PipelineReg::getNew(int type)
{
    if(DEBUG)
        printf("getPre(%u) = %u\n", type, (*(data+type))[0]);
    return (*(data+type))[0];
}

void PipelineReg::setNew(int type, int v)
{
    if(DEBUG)
        printf("setNew(%u) at %x\n", type, (*(data+type)));
    *(data+type)[0] = v;
    //data[type][0] = v;
}

void PipelineReg::setPre(int type, int v)
{
    if(DEBUG)
        printf("setPre(%u) at %x\n", type, &(*(data+type)[1]));
    (*(data+type))[1] = v;
    //data[type][0] = v;
}

void PipelineReg::dump()
{
    pDumpHelper->dump(this);
}

void PipelineReg::DumpHelper::setup(int count, const char **names, const int *indexs, int wid)
{
    this->count = count;
    this->names = names;
    this->indexs = indexs;
    this->csWid = wid;
}

void PipelineReg::DumpHelper::dump(PipelineReg* pReg)
{
    printf("\n%s :\n", name);
    for(int i=0;i<count;i++)
    {
        const char *rname = *(names+i);
        if(strcmp(rname, "Instruction") == 0)
        {
            int num = pReg->getPre(*(indexs+i));
            char outstr[50] = {0};
            for(int i=31;i>=0;i--)
              if(num & (1 << i))
                outstr[31-i] = '1';
              else
                outstr[31-i] = '0';
            printf("%s\t%s\n",rname, outstr);
        }
        else
        {
            printf("%s\t", rname);
            if(strlen(rname) <= 6)
                printf("\t");
            printf("%d\n", pReg->getPre(*(indexs+i)));
        }
    }
    if(isDumpCs)
    {
        pReg->csPre->dump(csWid);
    }
}

bool PipelineReg::isClear()
{
    for(int i=0;i<regLen;i++)
    {
        if(i == ignoreIdx)
            continue;
        if(data[i][1] != 0)
        {
            if(DEBUG)
                printf("isClear = false at %d\n", i);
            return false;
        }
    }
    if(csPre->getSignal() != 0)
        return false;
    return true;
}

void IF_ID::stall()
{
    //data[IF_ID::PC][1] = data[IF_ID::PC][0];
}



















