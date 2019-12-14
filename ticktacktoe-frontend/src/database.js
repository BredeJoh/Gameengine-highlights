import { resolve } from "path";
import { reject } from "q";

const azure = require("azure-storage");
const connectionString = "removed"
const tableService = azure.createTableService(connectionString);
const entGen = azure.TableUtilities.entityGenerator;

let dbManager = {
  addStatsToUser: (user, wins = 0, losses = 0, ties = 0) => {
    if (user === "" || user === "undefined" || user === null) return;

    let task = {
      PartitionKey: entGen.String("pkey"),
      RowKey: entGen.String(user),
      wins: entGen.Int32(0),
      losses: entGen.Int32(0),
      ties: entGen.Int32(0)
    };

    var query = new azure.TableQuery().top(1).where("RowKey eq ?", user);

    tableService.queryEntities("leaderboard", query, null, function(
      error,
      result,
      response
    ) {
      if (!error && result.entries.length > 0) {
        tableService.retrieveEntity("leaderboard", "pkey", user, function(
          err,
          result2,
          res
        ) {
          if (!err) {
            task.wins = entGen.Int32(result2.wins._ + wins);
            task.losses = entGen.Int32(result2.losses._ + losses);
            task.ties = entGen.Int32(result2.ties._ + ties);
            tableService.insertOrReplaceEntity("leaderboard", task, function(
              err,
              result3,
              res
            ) {
              if (!err) {
                // works
              }
            });
          }
        });
      } else {
        task.wins = entGen.Int32(wins);
        task.losses = entGen.Int32(losses);
        task.ties = entGen.Int32(ties);
        tableService.insertOrReplaceEntity("leaderboard", task, function(
          err,
          result2,
          res
        ) {
          if (!err) {
            // works
          }
        });
      }
    });
  },

  getTop10ByWins: () => {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        var query = new azure.TableQuery();

        tableService.queryEntities("leaderboard", query, null, function(
          error,
          result,
          response
        ) {
          if (!error && result.entries.length > 0) {
            let entries = result.entries;
            entries.sort(dbManager.compare);
            resolve(entries.slice(0, 10));
          } else reject(new Error("Something went wrong"));
        });
      }, 2500);
    });
  },

  compare: (a, b) => {
    const winsA = a.wins._;
    const winsB = b.wins._;

    let comparison = 0;
    if (winsA > winsB) comparison = -1;
    else if (winsB > winsA) comparison = 1;

    return comparison;
  }
};

export default dbManager;
