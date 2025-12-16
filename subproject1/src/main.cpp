#include <iostream>
#include <pqxx/pqxx> 

// using namespace std;
// using namespace pqxx;

// int main(int argc, char* argv[]) {
//    try {
//     // 连接到数据库
//       connection C("dbname = testDB1 user = lzy password = lzy \
//       hostaddr = 127.0.0.1 port = 5432");
//       if (C.is_open()) 
//       {
//          cout << "Opened database successfully: " << C.dbname() << endl;
//       } else 
//       {
//          cout << "Can't open database" << endl;
//          return 1;
//       }
//     //  C.close ();  //关闭数据库不用C.disconnect();

//     // 创建 表
//         // // 创建表
//         // std::string sql = "CREATE TABLE COMPANY_1("  \
//         // "ID INT PRIMARY KEY     NOT NULL," \
//         // "NAME           TEXT    NOT NULL," \
//         // "AGE            INT     NOT NULL," \
//         // "ADDRESS        CHAR(50)," \
//         // "SALARY         REAL );";
//         // // 插入数据
//         // std::string sql = "INSERT INTO COMPANY_1     (ID,NAME,AGE,ADDRESS,SALARY) "  \
//         //     "VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
//         //     "INSERT INTO COMPANY_1 (ID,NAME,AGE,ADDRESS,SALARY) "  \
//         //     "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
//         //     "INSERT INTO COMPANY_1 (ID,NAME,AGE,ADDRESS,SALARY)" \
//         //     "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
//         //     "INSERT INTO COMPANY_1 (ID,NAME,AGE,ADDRESS,SALARY)" \
//         //     "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

//         // // 获取和显示数据
//         // std::string sql = "SELECT * FROM COMPANY_1;";
//         // //创建一个非事务对象
//         // nontransaction N(C);
//         // //执行SQL语句
//         // result R( N.exec( sql ));
//         // //输出结果集
//         // for (const auto &row : R)
//         // {
//         //     cout << "ID = " << row[0].as<int>() << endl;
//         //     cout << "NAME = " << row[1].as<string>() << endl;
//         //     cout << "AGE = " << row[2].as<int>() << endl;
//         //     cout << "ADDRESS = " << row[3].as<string>() << endl;
//         //     cout << "SALARY = " << row[4].as<float>() << endl;
//         // }
//         // cout << "Operation done successfully" << endl;

//         // // 更新任意数据
//         // std::string update_sql = "UPDATE COMPANY_1 SET SALARY = 25000.00 WHERE ID=1;";
//         // // 创建一个事务对象
//         // work W(C);
//         // // 执行SQL语句
//         // W.exec( update_sql );
//         // W.commit();
//         // //查询更新后的数据
//         // std::string select_sql = "SELECT * FROM COMPANY_1 WHERE ID=1;";
//         // nontransaction N(C);
//         // result R( N.exec( select_sql ));
//         // for (const auto &row : R)
//         // {
//         //     cout << "ID = " << row[0].as<int>() << endl;
//         //     cout << "NAME = " << row[1].as<string>() << endl;
//         //     cout << "AGE = " << row[2].as<int>() << endl;
//         //     cout << "ADDRESS = " << row[3].as<string>() << endl;
//         //     cout << "SALARY = " << row[4].as<float>() << endl;
//         // }


//         //删除数据
//         std::string delete_sql = "DELETE FROM COMPANY_1 WHERE ID=2;";
//         work W(C);
//         W.exec( delete_sql );
//         W.commit();
//         cout << "Delete data successfully" << endl;
//         //查询删除后的数据
//         std::string select_sql = "SELECT * FROM COMPANY_1;";
//         nontransaction N(C);    
//         result R( N.exec( select_sql ));
//         for (const auto &row : R)
//         {
//             cout << "ID = " << row[0].as<int>() << endl;
//             cout << "NAME = " << row[1].as<string>() << endl;
//             cout << "AGE = " << row[2].as<int>() << endl;
//             cout << "ADDRESS = " << row[3].as<string>() << endl;
//             cout << "SALARY = " << row[4].as<float>() << endl;
//         }

//         C.close();

//    } catch (const std::exception &e) {
//       cerr << e.what() << std::endl;
//       return 1;
//    }
//     return 0;
// }

//多种查询方式演示
int main()
{
    try
    {
        // 连接到数据库
        pqxx::connection conn("dbname = testDB1 user = lzy password = lzy \
                    hostaddr = 127.0.0.1 port = 5432");

        // 创建一个事务对象
        pqxx::work W(conn);
        //  pqxx::work W{conn};

        // 使用query方法执行查询
        for (auto [name, salary] : W.query<std::string, int>(
        "SELECT name, salary FROM employee ORDER BY name"))
        {
            std::cout << name << " earns " << salary << ".\n";
        }

    }
    catch(const std::exception& e)
    {
        std::cerr <<"ERROR: " << e.what() << '\n';
    }
    
}