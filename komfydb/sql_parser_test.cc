#include <hsql/sql/SelectStatement.h>
#include <readline/history.h>
#include <readline/readline.h>

#include <iostream>

#include "hsql/SQLParser.h"
#include "hsql/util/sqlhelper.h"

int main() {
  char* query;
  while ((query = readline("KomfyDB> ")) != nullptr) {
    if (!strlen(query)) {
      continue;
    }
    add_history(query);
    hsql::SQLParserResult result;
    hsql::SQLParser::parse(query, &result);

    if (result.isValid() && result.size() > 0) {
      const hsql::SQLStatement* statement = result.getStatement(0);
      hsql::printStatementInfo(statement);
      const hsql::SelectStatement* select =
          static_cast<const hsql::SelectStatement*>(statement);
      std::cout << select->fromTable->type << "\n";
    } else {
      std::cout << "Parsing error [col " << result.errorColumn()
                << "]: " << result.errorMsg() << "\n";
    }
    free(query);
  }
}
