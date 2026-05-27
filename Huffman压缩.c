#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 100

//====================================================
// 保存 Huffman 编码表
// codes[数字] = 对应 Huffman编码
//====================================================

char codes[10][100];


//====================================================
// Huffman节点结构体
//====================================================

typedef struct Node
{
    char ch;                //字符

    int freq;               //频率

    struct Node *left;      //左孩子
    struct Node *right;     //右孩子

}Node;


//====================================================
// 创建节点
//====================================================

Node *createNode(char ch, int freq)
{
    //动态申请节点内存
    Node *newNode = (Node*)malloc(sizeof(Node));

    newNode->ch = ch;
    newNode->freq = freq;

    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}


//====================================================
// 最小堆结构体
//====================================================

typedef struct MinHeap
{
    Node *data[MAX];

    int size;

}MinHeap;


//====================================================
// 初始化堆
//====================================================

void initHeap(MinHeap *heap)
{
    heap->size = 0;
}


//====================================================
// 插入堆（上浮）
//====================================================

void insertHeap(MinHeap *heap, Node *node)
{
    //先放到最后
    int i = heap->size;

    heap->data[i] = node;

    heap->size++;


    //上浮
    //保证父节点频率始终小于孩子节点
    while(i > 0)
    {
        int parent = (i - 1) / 2;

        //如果父节点更大
        if(heap->data[parent]->freq >
     heap->data[i]->freq)
        {
            Node *temp = heap->data[parent];

            heap->data[parent] = heap->data[i];
            heap->data[i] = temp;

            //继续向上检查
            i = parent;
        }
        else
        {
            break;
        }
    }
}


//====================================================
// 取出最小节点（下沉）
//====================================================

Node *extractMin(MinHeap *heap)
{
    //堆顶一定是最小节点
    Node *minNode = heap->data[0];


    //最后节点补到根节点
    heap->data[0] =
        heap->data[heap->size - 1];

    heap->size--;


    //开始下沉调整
    int i = 0;

    while(1)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        int smallest = i;


        //寻找更小孩子
        if(left < heap->size &&
         heap->data[left]->freq <
         heap->data[smallest]->freq)
        {
            smallest = left;
        }

        if(right < heap->size &&
           heap->data[right]->freq <
           heap->data[smallest]->freq)
        {
            smallest = right;
        }


        //如果已经满足最小堆
        if(smallest == i)
        {
            break;
        }


        //交换节点
        Node *temp = heap->data[i];

        heap->data[i] =
            heap->data[smallest];

        heap->data[smallest] = temp;


        //继续向下检查
        i = smallest;
    }

    return minNode;
}


//====================================================
// 构造 Huffman树
//====================================================

Node *buildHuffmanTree(MinHeap *heap)
{
    while(heap->size > 1)
    {
        //取两个最小节点
        Node *left = extractMin(heap);
        Node *right = extractMin(heap);


        //创建新的父节点
        Node *parent =
            createNode('\0',
                       left->freq + right->freq);


        //挂左右子树
        parent->left = left;
        parent->right = right;


        //重新放回堆中
        insertHeap(heap, parent);
    }


    //最终剩余节点就是 Huffman树根节点
    return extractMin(heap);
}


//====================================================
// DFS生成 Huffman编码
//====================================================

void generateCodes(Node *root,
                   char code[],
                   int depth)
{
    if(root == NULL)
    {
        return;
    }


    //到达叶子节点
    //说明真正找到字符
    if(root->left == NULL &&
       root->right == NULL)
    {
        code[depth] = '\0';


        //保存编码表
        strcpy(codes[root->ch - '0'],
               code);


        printf("%c : %s\n",
               root->ch,
               code);

        return;
    }


    //左边路径记为0
    code[depth] = '0';

    generateCodes(root->left,
                  code,
                  depth + 1);


    //右边路径记为1
    code[depth] = '1';

    generateCodes(root->right,
                  code,
                  depth + 1);
}


//====================================================
// 主函数
//====================================================

int main()
{
    //====================================================
    // 初始化最小堆
    //====================================================

    MinHeap heap;

    initHeap(&heap);


    //====================================================
    // 打开 dgb 文件
    //====================================================

    FILE *fp;

    fp = fopen("knot.dgb", "rb");

    if(fp == NULL)
    {
        printf("文件打开失败\n");

        return 1;
    }


    //====================================================
    // 提取数字数据
    //====================================================

    char ch;

    int data[256];

    int count = 0;


    //逐字符读取文件
    while(fread(&ch,
                sizeof(char),
                1,
                fp))
    {
        //只提取数字字符
        if(ch >= '0' && ch <= '9')
        {
            //ASCII转真正数字
            data[count] = ch - '0';

            count++;
        }
    }


    fclose(fp);


    //====================================================
    // 统计频率
    //====================================================

    int freq[10] = {0};

    for(int i = 0; i < count; i++)
    {
        freq[data[i]]++;
    }


    //====================================================
    // 输出频率统计
    //====================================================

    printf("\n频率统计:\n");

    for(int i = 0; i < 10; i++)
    {
        printf("%d : %d次\n",
               i,
               freq[i]);
    }


    //====================================================
    // 创建 Huffman节点
    //====================================================

    for(int i = 0; i < 10; i++)
    {
        if(freq[i] > 0)
        {
            insertHeap(
                &heap,
                createNode('0' + i,
                           freq[i])
            );
        }
    }


    //====================================================
    // 构造 Huffman树
    //====================================================

    Node *root =
        buildHuffmanTree(&heap);


    //====================================================
    // 生成 Huffman编码
    //====================================================

    printf("\nHuffman编码:\n");

    char code[100];

    generateCodes(root,
                  code,
                  0);


    //====================================================
    // 创建压缩文件
    //====================================================

    FILE *out;

    out = fopen("wayne.dgbz", "wb");


    //====================================================
    // 写文件头
    // 标记当前文件是 Huffman压缩文件
    //====================================================

    fwrite("DGBZ",
           sizeof(char),
           4,
           out);


    //====================================================
    // 保存原始数据长度
    // 防止最后补0导致解码错误
    //====================================================

    fwrite(&count,
           sizeof(int),
           1,
           out);


    //====================================================
    // 保存编码表
    //====================================================

    fprintf(out, "\n");

    for(int i = 0; i < 10; i++)
    {
        if(strlen(codes[i]) > 0)
        {
            fprintf(out,
                    "%d %s\n",
                    i,
                    codes[i]);
        }
    }


    //====================================================
    // bit压缩
    //====================================================

    unsigned char byte = 0;

    int bitCount = 0;


    //遍历原始数据
    for(int i = 0; i < count; i++)
    {
        //取当前数字编码
        char *codeStr = codes[data[i]];


        //遍历编码字符串
        for(int j = 0;
            codeStr[j] != '\0';
            j++)
        {
            //字符转bit
            int bit = codeStr[j] - '0';


            //左移腾位置
            //再拼接新的bit
            byte = (byte << 1) | bit;

            bitCount++;


            //满8bit写入文件
            if(bitCount == 8)
            {
                fwrite(&byte,
                       sizeof(unsigned char),
                       1,
                       out);

                byte = 0;

                bitCount = 0;
            }
        }
    }


    //====================================================
    // 处理最后不足8bit情况
    //====================================================

    if(bitCount > 0)
    {
        //补0到8bit
        byte <<= (8 - bitCount);

        fwrite(&byte,
               sizeof(unsigned char),
               1,
               out);
    }


    fclose(out);


    printf("\n压缩完成！\n");

    return 0;
}
