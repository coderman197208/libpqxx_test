#include <iostream>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

int main(int argc, char *argv[])
{
    try
    {
        // 连接到数据库
        connection conn("dbname = testDB1 user = lzy password = lzy \
      hostaddr = 127.0.0.1 port = 5432");
        if (conn.is_open())
        {
            cout << "Opened database successfully: " << conn.dbname() << endl;
        }
        else
        {
            cout << "Can't open database" << endl;
            return 1;
        }

        // // 1、不创建事务对象和非事务对象,使用参数化查询数据
        // {
        //     int id = 1;
        //     std::string sql = "SELECT * FROM COMPANY_1 WHERE ID = $1;";
        //     pqxx::nontransaction ntx(conn);
        //     // 弃用：result result = ntx.exec_params(sql, id);
        //     result result = ntx.exec(sql, pqxx::params{id});
        //     if (result.size() > 0)
        //     {
        //         const row row = result[0];
        //         cout << "ID = " << row["id"].as<int>() << endl;
        //         cout << "NAME = " << row["name"].as<string>() << endl;
        //         cout << "AGE = " << row["age"].as<int>() << endl;
        //         cout << "ADDRESS = " << row["address"].as<string>() << endl;
        //         cout << "SALARY = " << row["salary"].as<float>() << endl;
        //     }
        //     else
        //     {
        //         cout << "No data found for ID = " << id << endl;
        //     }
        // }

        // // 2、创建非事务对象 进行Update操作
        // {
        //     pqxx::nontransaction ntx(conn);
        //     std::string update_sql = "UPDATE COMPANY_1 SET SALARY = $1 WHERE ID = $2;";
        //     float new_salary = 50000.0;
        //     int update_id = 1;
        //     // ❌ 旧的弃用方式
        //     // result result1 = ntx.exec_params(update_sql, new_salary, update_id);
        //     result result1 = ntx.exec(update_sql, pqxx::params{new_salary, update_id});
        //     if (result1.affected_rows() == 0)
        //     {
        //         cout << "No rows updated for ID = " << update_id << endl;
        //     }
        //     else
        //     {
        //         cout << "Rows updated successfully for ID = " << update_id << endl;
        //     }
        // }

        // 3、获取表的记录数
        {

            pqxx::nontransaction ntx(conn);
            std::string count_sql = "SELECT COUNT(*) FROM COMPANY_1;";
            result count_result = ntx.exec(count_sql);
            cout << "Total records in COMPANY_1: " << count_result[0][0].as<int>() << endl;
        }

        // 4、获取age的最大值
        {
            pqxx::nontransaction ntx(conn);
            std::string max_age_sql = "SELECT MAX(AGE) FROM COMPANY_1;";
            result max_age_result = ntx.exec(max_age_sql);
            cout << "Max age in COMPANY_1: " << max_age_result[0][0].as<int>() << endl;
        }

        conn.close();
    }
    catch (const std::exception &e)
    {
        cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
