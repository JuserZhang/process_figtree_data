#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SAVE_TMP_FILE "./.save_tmp.txt"
//#define RESULT_FILE "./result.txt"
#define MAX_LEN 8192
int g_len=0;
int g_count=0;

//忽略空格
char *ignor_space(char *str)
{
	char *dest=calloc(1,g_len);
	char *p = str;
	char *q=dest;

	while(*p){
		if(*p == ' '){
			p++;
			continue;
		}
		*q++ = *p++;
	}
	free(str);
	return dest;
}

//忽略: ,/)中间内容
char *ignor3_str(char *str)
{
	char *step3_str=calloc(1,g_len);
	char *p = str;
	char *q=step3_str;
	int clean_char_flag=0;
	while(*p){
		if(*p == ':'){
			clean_char_flag=1;
			p++;
			continue;
		}else if(*p == ',' || *p == ')'){
			clean_char_flag=0;
			*q++ = *p++;
			continue;
		}

		if(clean_char_flag == 1){
			p++;
			continue;
		}

		*q++ = *p++;
	}
	free(str);
	return step3_str;
}

//忽略[]内容
char *ignor2_str(char *str)
{
	char *step2_str=calloc(1,g_len);
	char *p = str;
	char *q=step2_str;
	int clean_char_flag=0;
	while(*p){
		if(*p == '['){
			clean_char_flag=1;
			p++;
			continue;
		}else if(*p == ']'){
			clean_char_flag=0;
			p++;
			continue;
		}

		if(clean_char_flag == 1){
			p++;
			continue;
		}

		*q++ = *p++;
	}
	free(str);
	return step2_str;
}

//忽略{}内容
char *ignor1_str(char *str)
{
	char *step1_str=calloc(1,g_len);
	char *p = str;
	char *q=step1_str;
	int clean_char_flag=0;
	while(*p){
		if(*p == '{'){
			clean_char_flag=1;
			p++;
			continue;
		}else if(*p == '}'){
			clean_char_flag=0;
			p++;
			continue;
		}

		if(clean_char_flag == 1){
			p++;
			continue;
		}

		*q++ = *p++;
	}
	free(str);
	return step1_str;
}

//将格式化后的字符串分离然后写入文件
int separ_str_to_file(char *line,FILE *fp)
{
	char l_dest[MAX_LEN]={0};
	char r_dest[MAX_LEN]={0};
	char newline[MAX_LEN]={0};
	char *p = line;
	char *q=l_dest;

	while(*p){
		if(*p == ','){
			q=r_dest;
			p++;
			continue;
		}
		if(*p == '(' || *p == ')'){
			p++;
			continue;
		}
		*q++ = *p++;
	}
	sprintf(newline,"%s,%s\n",l_dest,r_dest);
	fputs(newline, fp);
	return 0;
}

FILE* open_file_write(char *file)
{
	FILE* pfiles = fopen(file, "w+");
	if (pfiles == NULL){
		printf("Open %s fail.\n",file);
		return NULL;
	}
	return pfiles;
}

FILE* open_file_read(char *file)
{
	FILE* pfiles = fopen(file, "r");
	if (pfiles == NULL){
		printf("Open %s fail.\n",file);
		return NULL;
	}
	return pfiles;
}

void close_file(FILE* pfile)
{
	if(pfile){
		fclose(pfile);
		pfile = NULL;
	}
}

char* read_file(char * path, int *length)
{
	FILE *pfile;
	char *data;
 
	pfile = fopen(path, "r");
	if (pfile == NULL)
	{
		return NULL;
	}
	fseek(pfile, 0, SEEK_END);
	*length = ftell(pfile);
	data = (char *)malloc((*length + 1) * sizeof(char));
	rewind(pfile);
	*length = fread(data, 1, *length, pfile);
	data[*length] = '\0';
	fclose(pfile);
	return data;
}

/*获取叶子对*/
int get_leaf(char *src, FILE* save_tmp)
{
	char *p_rear = src;
	char *p_front = src;
	char tmp[MAX_LEN]={'\0'};
	char *q_tmp = tmp;
	int num = 0;
	int record_flag = 0;

	while (*p_front){
		if(*p_front == '('){
			p_rear = p_front;
			record_flag = 1;
		}
		if(*p_front == ')' && record_flag == 1){
			memset(tmp,'\0',MAX_LEN);
			record_flag = 0;
			q_tmp = tmp;
			while (p_rear != p_front){
				*q_tmp=*p_rear;
				if(*p_rear == '('){
					*p_rear = ' ';
				}else if( *p_rear == ','){
					*p_rear = '+';
				}
				q_tmp++;
				p_rear++;
			}
			*q_tmp =*p_rear;
			if(*p_rear == ')'){
				*p_rear = ' ';
			}
			q_tmp++;
			p_rear++;
			fputs(tmp, save_tmp); 
			fputc('\n', save_tmp); 
			num++;
		}
		p_front++;
	}
	return num;
}

//判断是否存在数据需要提取
int is_left_pair(char *dest)
{
	char *p = dest;
	while (*p){
		if(*p == '(' || *p == ')'){
			return 1;
		}
		p++;
	}
	return 0;
}

int main(int argc,char **argv)
{
	char *file = argv[1];
	char *save_result_file = argv[2];
	char *txt = NULL;
	char *step1_str = NULL;
	char *step2_str = NULL;
	char *step3_str = NULL;
	char *dest = NULL;
	char *line = NULL;
	int num = 0;

	FILE *save_tmp = NULL;
	FILE *save_result = NULL;

	save_tmp = open_file_write(SAVE_TMP_FILE);
	if(!save_tmp){
		printf("open %s failed.",SAVE_TMP_FILE);
		exit(-1);
	}

	txt = read_file(file,&g_len);
	//printf("len:[%d] txt: %s\n",g_len,txt);

	/*格式化文本*/
	step1_str = ignor1_str(txt);
	//printf("step1_str: %s\n",step1_str);
	step2_str = ignor2_str(step1_str);
	//printf("step2_str: %s\n",step2_str);
	step3_str = ignor3_str(step2_str);
	//printf("step3_str: %s\n",step3_str);
	dest = ignor_space(step3_str);
	//printf("step4_str: %s\n",dest);



	while(1){
		if(is_left_pair(dest) == 0){
			break;
		}
		//从端点获取信息对
		num = get_leaf(dest,save_tmp);
		dest = ignor_space(dest);
		g_count+=num;
	}
	printf("There are %d scenarios\n",g_count);
	close_file(save_tmp);


	//将保存的文件逐行提取格式化输出
	save_tmp = open_file_read(SAVE_TMP_FILE);
	save_result = open_file_write(save_result_file);
	if(!save_result){
		printf("open %s failed.",save_result_file);
		exit(-1);
	}

	line = calloc(1,g_len);

	while (!feof(save_tmp))
	{
		memset(line,'\0',g_len);
		fgets(line,g_len,save_tmp);

		if(strlen(line) == 0)
			continue;
		line[strlen(line)-1] = '\0'; //去掉换行符
		separ_str_to_file(line,save_result);
	}

	close_file(save_tmp);
	close_file(save_result);
	printf("Result save to file:%s\n",save_result_file);
	return 0;
}