#include "BrowseHistoryController.h"
#include "utils/ResponseUtil.h"
#include <drogon/orm/DbClient.h>

void BrowseHistoryController::getHistory(const HttpRequestPtr& req,
                                         std::function<void(const HttpResponsePtr&)>&& callback) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    auto db = app().getDbClient();

    db->execSqlAsync(
        "SELECT "
        "  b.id AS board_id, "
        "  b.board_name, "
        "  p.id AS post_id, "
        "  p.title, "
        "  h.browse_time "
        "FROM browse_history h "
        "JOIN post p ON h.post_id = p.id "
        "JOIN board b ON p.board_id = b.id "
        "WHERE h.user_id = ? "
        "ORDER BY b.id, h.browse_time DESC",
        [callback](const orm::Result &result) {
            std::map<int, Json::Value> boardMap;
            for (const auto &row : result) {
                int boardId = row["board_id"].as<int>();
                std::string boardName = row["board_name"].as<std::string>();

                Json::Value post;
                post["post_id"] = row["post_id"].as<int>();
                post["title"] = row["title"].as<std::string>();
                post["browse_time"] = row["browse_time"].as<std::string>();

                if (boardMap.find(boardId) == boardMap.end()) {
                    Json::Value board;
                    board["board_id"] = boardId;
                    board["board_name"] = boardName;
                    board["posts"] = Json::arrayValue;
                    boardMap[boardId] = board;
                }
                boardMap[boardId]["posts"].append(post);
            }

            Json::Value data(Json::arrayValue);
            for (auto &pair : boardMap) {
                data.append(pair.second);
            }
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            LOG_ERROR << "获取浏览历史失败: " << e.base().what();
            callback(ResponseUtil::error("获取浏览历史失败"));
        },
        userId);
}