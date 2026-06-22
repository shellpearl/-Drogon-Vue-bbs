#include "BoardController.h"
#include "utils/ResponseUtil.h"

void BoardController::listBoards(const HttpRequestPtr& req,
                                 std::function<void(const HttpResponsePtr&)>&& callback) {
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT id, board_name, description, created_at FROM board ORDER BY id",
        [callback](const orm::Result &result) {
            Json::Value data(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value board;
                board["id"] = row["id"].as<int>();
                board["board_name"] = row["board_name"].as<std::string>();
                board["description"] = row["description"].as<std::string>();
                board["created_at"] = row["created_at"].as<std::string>();
                data.append(board);
            }
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("获取板块失败"));
        });
}