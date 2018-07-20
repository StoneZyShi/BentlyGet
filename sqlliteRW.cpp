#include "sqlliteRW.h"
//#include "..\include\easySQLite\easySqlite.h"
//#include "..\include\easySQLite\SqlCommon.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "getData.h"

using namespace std;

sqlliteRW::sqlliteRW(WString fileName)
{
#ifdef MY_DEBUG
	pri(fileName.GetWCharCP());
#endif // MY_DEBUG
	obj.id = 0;
	this->fileName = ws2sT(fileName);
	replace_all(this->fileName, "./_./()--", "");
	replace_all(this->fileName, " ", "");
	this->objNum = 0;
}


sqlliteRW::~sqlliteRW()
{
}

//static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
//	int i;
//	for (i = 0; i<argc; i++) {
//		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//	}
//	printf("\n");
//	return 0;
//}

bool sqlliteRW::creatObjTable()
{
	sqlite3 *db;
	char *zErrMsg = 0;

	if ((_access(this->fileName.c_str(), 0)) != -1)
	{
		if (remove(this->fileName.c_str()) != 0)
		{
			pri(L"删除文件失败");
		}
	}

	int result = sqlite3_open(toUtf8(this->fileName).c_str(), &db);
	if (!result)
	{
#ifdef MY_DEBUG
		pri(L"Open the database sqlite.db sucessfully\n");
#endif
	}

	/* Create SQL statement */
	char* sql = "CREATE TABLE obj_table("  \
		"id                    INT PRIMARY KEY  NOT NULL," \
		"hostfile_name         CHAR(255)," \
		"elem_guid_infile      CHAR(100)," \
		"id_infile             INT," \
		"obj_file_Data         BLOB," \
		"gudi                  INT );";
	result = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
	if (result != SQLITE_OK) {
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);
		pri(L"SQL error:创建失败");
		sqlite3_free(zErrMsg);
	}
	else
	{
#ifdef MY_DEBUG
		pri(L"Table created successfully\n");
#endif
	}

	sqlite3_close(db);

	return false;
}

bool sqlliteRW::addData(WString STR)
{
	sqlite3 *db;
	sqlite3_stmt *stat;
	std::string str = ws2s(STR.GetWCharCP());

#ifdef MY_OBJ
	char num[256] = "\0";
	sprintf(num, "%d", this->objNum++);
	std::string objName = this->fileName.substr(0, this->fileName.rfind(".")) + "_"+num + ".obj";
	if ((_access(objName.c_str(), 0)) != -1)
	{
		if (remove(objName.c_str()) != 0)
		{
			pri(L"删除文件失败");
		}
	}
	ofstream out(objName, ios::app);
	if (out.is_open())
	{
		out << str.c_str() << endl;
		out.close();
	}
#endif // 

#ifdef MY_DEBUG
	pri(L"设置数据");
#endif

	int result = sqlite3_open(toUtf8(this->fileName).c_str(), &db);
	if (!result)
	{
		//pri(L"addData");
		sqlite3_prepare(db, "insert into obj_table (id,hostfile_name,id_infile,obj_file_Data) values(?,?,?,?);", (int)-1, &stat, (int)0);
#ifdef MY_DEBUG
		//WString msg;
		//msg.Sprintf(L"fileName = %s ID=%d   obj.hostfile_name.c_str()=%s   obj.id_infile=%d\n", fileName,obj.id, obj.hostfile_name.c_str(), obj.id_infile);
		//pri(msg.GetWCharCP());
#endif
		//pri(L"开始插入");
		sqlite3_bind_int(stat, 1, obj.id);
		sqlite3_bind_text(stat, 2, toUtf8(obj.hostfile_name).c_str(),(int)(toUtf8(obj.hostfile_name).length() + 1),NULL);
		sqlite3_bind_int(stat, 3, obj.id_infile);
		sqlite3_bind_blob(stat, 4, str.c_str(), (int)(str.length() + 1), NULL);
		
		sqlite3_step(stat);
		sqlite3_finalize(stat);
		sqlite3_close(db);

#ifdef MY_DEBUG
		//pri(L"插入完成");
#endif
	}
	else
	{
		pri(s2wsT("打开sql失败："+ fileName).GetWCharCP());
	}

	return true;
}

void sqlliteRW::set_id()
{
	this->obj.id++;
}

void sqlliteRW::set_hostfile_name(WString hostfile_name)
{
	this->obj.hostfile_name = ws2s(hostfile_name.GetWCharCP());
}

void sqlliteRW::set_elem_guid_infile(WString elem_guid_infile)
{
	this->obj.elem_guid_infile = ws2s(elem_guid_infile.GetWCharCP());
}

void sqlliteRW::set_id_infile(int id_infile)
{
	this->obj.id_infile = id_infile;
}

void sqlliteRW::set_guid(int guid)
{
	this->obj.guid = guid;
}


