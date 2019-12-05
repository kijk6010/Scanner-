#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define KEY 23
#define ID_LENGTH 12
#define MAX 1024
char id[ID_LENGTH];

struct tokenT {
	int number; //토큰 번호
	union {
		char id[ID_LENGTH];
		int num;
	}value;//토큰 값
};

enum T_Symbol{
	tnull = -1,//token number = -1,
	tnot, tnotequ, tmod, tmodAssign, tident, tnumber, 
  // 0      1        2      3          4       5
	tand, tlparen, trparen, tmul, tmulAssign, tplus, 
  // 6      7        8       9        10       11
	tinc, taddAssign, tcomma, tminus, tdec, tsubAssign, 
  // 12        13       14      15     16       17 
	tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign, 
  // 18        19         20        21     22      23
	tequal, tgreat, tgreate, tlbracket, trbracket, teof, 
  // 24       25       26       27         28       29
	tconst, telse, tif, tint, treturn, tvoid, 
  // 30       31    32   33     34       35
	twhile, tdo, tdouble, textern, tfloat, tfor,
  // 36      37     38      39       40     41 -->37번부터 추가된 부분
	tgoto, tlong, tshort, tstatic, tstruct, tswitch,
  // 42      43     44      45       46        47
	ttypedef, tunion, tunsigned, tenum, tregister,
  // 48         49       50        51       52
	tlbrace, tor, trbrace
  //  53      54    55
};

const char *keyword[KEY] = { //예약어의 string값을 갖는 배열
	  "const", "else", "if", "int", "return", "void", "while","do","double","extern","float","for",
	  "goto", "long","short","static","struct","switch","typedef","union","unsigned","enum","register"
};

enum T_Symbol OPcode[KEY]{ //각 예약어의 토큰 번호를 갖는 배열
	 tconst, telse, tif, tint, treturn, tvoid, twhile, tdo, tdouble, textern, tfloat,tfor, tgoto,
	 tlong, tshort, tstatic, tstruct, tswitch, ttypedef,tunion, tunsigned, tenum, tregister
};

//문자 에러 처리
void lexicalError(int n) {
	printf(" *** Lexical Error : ");
	switch (n) {
	case 1: printf("an identifier length must be less than 12.\n");
		break;
	case 2: printf("next character must be &.\n");
		break;
	case 3: printf("next character must be |.\n");
		break;
	case 4: printf("invalid character!!!\n");
		break;
	}
}

int superLetter(char ch) {
	if (isalpha(ch) || ch == '_') return 1;
	else return 0;
}

int superLetterOrDigit(char ch) {
	if (isalnum(ch) || ch == '_')return 1;
	else return 0;
}

int hexValue(char ch) {
	switch (ch) {
		case'0': case'1': case'2': case'3': case'4':
		case'5': case'6': case'7': case'8': case'9':
			return(ch - '0');
		case'A': case'B': case'C': case'D': case'E': case'F':
			return(ch - 'A' + 10);
		case'a': case'b': case'c': case'd': case'e': case'f':
			return(ch - 'a' + 10);
		default: return -1;
	}
}

int getIntNum(char firstChar, FILE* input) {
	int num = 0;
	int value;
	char ch;

	if (firstChar != '0') {
		ch = firstChar;
		do {
			num = 10 * num + (int)(ch - '0');
			ch = fgetc(input);
		} while (isdigit(ch));
	}
	else {
		ch = fgetc(input);
		if ((ch >= '0') && (ch <= '7')) {
			do {
				num = 8 * num + (int)(ch - '0');
				ch = fgetc(input);
			} while ((ch >= '0') && (ch <= '7'));
		}
		else if ((ch == 'X' || (ch == 'x'))) {
			while ((value = hexValue(ch = fgetc(input))) != -1)
				num = 16 * num + value;
		}
		else num = 0;
	}
	ungetc(ch, stdin);
	return num;
}

struct tokenT scanner(FILE* input) {
	struct tokenT token;
	int i, index;
	char ch;
	
	token.number = tnull;

	do {
		while (isspace(ch = fgetc(input)));
		if (superLetter(ch)) {
			i = 0;
			do {
				if (i < ID_LENGTH)id[i++] = ch;
				ch = fgetc(input);
			} while (superLetterOrDigit(ch));
			if (i >= ID_LENGTH) lexicalError(1);
			id[i] = '\0';
			ungetc(ch, stdin);

			for (index = 0; index < KEY; index++)
				if (!strcmp(id, keyword[index])) break;
			if (index < KEY) token.number = OPcode[index];
			else {
				token.number = tident;
				strcpy(token.value.id, id);
			}
		}

		else if (isdigit(ch)) {
			token.number = tnumber;
			token.value.num = getIntNum(ch, input);
		}
		else switch (ch) {
		case'/':
			ch = fgetc(input);
			if (ch == '*') {
				do {
					while (ch != '*')ch = fgetc(input);
					ch = fgetc(input);
				} while (ch != '/');
			}
			else if (ch == '/')
				while (fgetc(input) != '\n');
			else if (ch == '=') token.number = tdivAssign;
			else {
				token.number = tdiv;
				ungetc(ch, stdin);
			}
			break;

		case '!':
			ch = fgetc(input);
			if (ch == '=') token.number = tnotequ;
			else {
				token.number = tnot;
				ungetc(ch, stdin);
			}
			break;

		case '%':
			ch = fgetc(input);
			if (ch == '=')
				token.number = tmodAssign;
			else {
				token.number = tmod;
				ungetc(ch, stdin);
			}
			break;
		
		case '&':
			ch = fgetc(input);
			if (ch == '&') token.number = tand;
			else {
				lexicalError(2);
				ungetc(ch, stdin);
			}
			break;
		
		case '*':
			ch = fgetc(input);
			if (ch == '=')token.number = tmulAssign;
			else {
				token.number = tmul;
				ungetc(ch, stdin);
			}
			break;

		case '+':
			ch = fgetc(input);
			if (ch == '+')token.number = tinc;
			else if (ch == '=')token.number = taddAssign;
			else {
				token.number = tplus;
				ungetc(ch, stdin);
			}
			break;

		case '-':
			ch = fgetc(input);
			if (ch == '-')token.number = tdec;
			else if (ch == '=')token.number = tsubAssign;
			else {
				token.number = tminus;
				ungetc(ch, stdin);
			}
			break;

		case '<':
			ch = fgetc(input);
			if (ch == '=')token.number = tlesse;
			else { 
				token.number = tlesse;
				ungetc(ch, stdin);
			}
			break;

		case '=':
			ch = fgetc(input);
			if (ch == '=')token.number = tequal;
			else {
				token.number = tassign;
				ungetc(ch, stdin);
			}
			break;

		case '>':
			ch = fgetc(input);
			if (ch == '=')token.number = tgreate;
			else {
				token.number = tgreat;
				ungetc(ch, stdin);
			}
			break;

		case '|':
			ch = fgetc(input);
			if (ch == '|') token.number = tor;
			else {
				lexicalError(3);
				ungetc(ch, stdin);
			}
			break;
		case '(':token.number = tlparen; break;
		case ')':token.number = trparen; break;
		case ',':token.number = tcomma; break;
		case ';':token.number = tsemicolon; break;
		case '[':token.number = tlbracket; break;
		case ']':token.number = trbracket; break;
		case '{':token.number = tlbrace; break;
		case '}':token.number = trbrace; break;
		case EOF:token.number = teof; break;

		default: {
			printf("Current character: %c", ch);
			lexicalError(4);
			break;
		}
		}
	} while (token.number == tnull);
	return token;
}

//token Value를 출력하기 위한 Switch 문
const char* tokenValue(int tokenNum)
{
	switch (tokenNum) {
	case tnot: return "!";
	case tnotequ: return "!=";
	case tmod: return "%";
	case tmodAssign: return "%=";
	case tand: return "&&";
	case tlparen: return "(";
	case trparen: return ")";
	case tmul: return "*";
	case tmulAssign: return "*=";
	case tplus: return "+";
	case tinc: return "++";
	case taddAssign: return "+=";
	case tcomma: return ",";
	case tminus: return "-";
	case tdec: return "--";
	case tsubAssign: return "-=";
	case tdiv: return "/";
	case tdivAssign: return "/=";
	case tsemicolon: return ";";
	case tless: return "<";
	case tlesse: return "<=";
	case tassign: return "=";
	case tequal: return "==";
	case tgreat: return ">";
	case tgreate: return ">=";
	case tlbracket: return "[";
	case trbracket: return "]";
	case teof: return "EOF";
	case tconst: return "const";
	case telse: return "else";
	case tif: return "if";
	case tint: return "int";
	case treturn: return "return";
	case tvoid: return "void";
	case twhile: return "while";
	case tdo: return "do";
	case tdouble: return "double";
	case textern: return "extern";
	case tfloat: return "float";
	case tfor: return "for";
	case tgoto: return "goto";
	case tlong: return "long";
	case tregister: return "register";
	case tshort: return "short";
	case tstatic: return "static";
	case tstruct: return "struct";
	case tswitch: return "switch";
	case ttypedef: return "typedef";
	case tunion: return "union";
	case tunsigned: return "unsigned";
	case tenum: return "enum";
	case tlbrace: return "{";
	case tor: return "||";
	case trbrace: return "}";
	default: printf("not token\n");
		return NULL;
	}
}

//파일에 담겨 있는 value값을 공백으로 나눠 배열에 담아주는 함수
int retToken(char* ori[], char *inp) {
	int i = 0;
	char* ptr = strtok(inp, " ");
	while (ptr != NULL) {
		ori[i] = ptr;
		ptr = strtok(NULL, " ");
		i++;
	}
	return i;
}

int main(void) {
	//OPcode Table 출력 부분
	printf("\t*OPcode Table*\n");
	printf("------------------------------\n");
	printf(" token number\ttoken value\n");
	printf("------------------------------\n");
	for (int i = 0; i < KEY; i++) {
		printf("\t%d\t  %s\n", i+30,keyword[i]);
	}
	printf("------------------------------\n\n");

	//Symbol table 출력을 위한 연산 부분
	FILE *fp1 = fopen("input.txt", "r");
	FILE *fp2 = fopen("temp.txt", "w");

	struct tokenT token;
	if((fp1 == NULL) || (fp2 == NULL)){
		printf("파일 오픈 오류!!!\n");
		exit(1);
	}

	int T2[100]; // 정수 정렬 후 Counting을 위한 배열
	int arr3[MAX] = {0}; //기호들 Counting을 위한 배열
	int index2 = -1;

	while (!feof(fp1)){
		token = scanner(fp1);
		if (token.number == tident) {
			//변수 값을 파일에 담아준다. 배열로 바로 담을시 쓰레기 값이 출력됨.
			fwrite(token.value.id,strlen(token.value.id),1,fp2);
			fputs(" ",fp2);
		}
		else if (token.number == tnumber) {
			index2++;
			T2[index2] = token.value.num;
		}
		else { 
			arr3[token.number+1]++;
		}
	}
	fclose(fp1);
	fclose(fp2);

	//다시 파일을 열어 값을 꺼내 배열에 넣어준다.(쓰레기값X)
	FILE *fp3 = fopen("output.txt", "r");
	int buf_size = MAX * MAX;
	int index = 0;
	char* inp;
	char* ptr;
	char* ori[MAX];
	inp = (char*)malloc(buf_size);
	while (fgets(inp, buf_size, fp3)) {
		index = retToken(ori, inp);
	}
	fclose(fp3);

	//문자열 오름차순 정렬_(쉬운 문자열 카운팅을 위해)
	char* ptemp;
	for (int data = 0; data < index-1; data++) {
		for (int i = 0; i < index - 1 - data; i++) {
			if (strcmp(ori[i], ori[i + 1]) > 0) {
				ptemp = ori[i];
				ori[i] = ori[i + 1];
				ori[i + 1] = ptemp;
			}
		}
	}
	//문자열 카운팅을 위한 연산
	char* temp1 = ori[0];
	int j = 0;
	int arr1[MAX] = {};//count
	char* arr1V[MAX] = {};//value
	for (int i = 0; i < index; i++) {
		if (strcmp(temp1,ori[i])==0) {
			arr1[j]++;
			arr1V[j] = temp1;
		}
		else {
			arr1V[j] = temp1;
			temp1 = ori[i];
			j++;
			arr1[j] = 1;
		}
	}

	printf("\t       *Symbol Table*\n");
	printf("-----------------------------------------------\n");
	printf(" token number\ttoken value\t  token count\n");
	printf("-----------------------------------------------\n");
	for (int i = 0; i < j; i++) {
		printf("\t%d\t %7s\t\t%d\n ", tident, arr1V[i], arr1[i]);
	}
	//정수 오름차순 정렬_(쉬운 카운팅을 위해)
	int temp = 0;
	for (int i = 0; i < index2; i++) {
		for (int j = 0; j < (index2)-i; j++) {
			if (T2[j] > T2[j + 1]) {
				temp = T2[j];
				T2[j] = T2[j + 1];
				T2[j + 1] = temp;
			}
		}
	}
	//카운팅 연산
	int arr2[30] = {};//count
	int arr2V[30] = {};//value
	int temp2 = T2[0];
	int k = 0;
	for (int i = 0; i < index2 + 1; i++) {
		if (temp2 == T2[i]) {
			arr2[k]++;
			arr2V[k] = temp2;
		}
		else if (i == index2) {
			k++;
			arr2V[k] = T2[i];
			arr2[k] = 1;
		}
		else {
			arr2V[k] = temp2;
			temp2 = T2[i];
			k++;
			arr2[k] = 1;
		}
	}
	for (int i = 0; i < k + 1; i++) {
		printf("\t%d\t %7d\t\t%d\n ", tnumber, arr2V[i], arr2[i]);
	}
	for (int i = 0; i < MAX; i++) {
		if (arr3[i] != 0) {
			printf("\t%d\t %7s\t\t%d\n ", i-1,tokenValue(i-1),arr3[i]);
		}
	}
	printf("-----------------------------------------------\n");
		system("pause");
		return 0;
	}
