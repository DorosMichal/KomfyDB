#include "komfydb/optimizer/table_stats.h"

#include "absl/status/statusor.h"
#include "komfydb/utils/status_macros.h"

#include "komfydb/optimizer/int_histogram.h"
#include "komfydb/optimizer/string_histogram.h"



namespace {
  using komfydb::storage::TableIterator;
  using komfydb::transaction::TransactionId;

}

namespace komfydb::optimizer {

absl::StatusOr<TableStats> TableStats::Create(int table_id,
                                              int io_cost_per_page, std::shared_ptr<Catalog> catalog,
      std::shared_ptr<BufferPool> bufferpool) {
  // read whole table
  TableIterator table_iterator(TransationId(), table_id, catalog, bufferpool);
  if(!table_iterator.Open().ok()){
    LOG(ERROR) << "Failed to open table iterator to read statistics from table with id " << table_id;
    return 1;
  }

  std::vector<std::unique_ptr<Record>> records;
  ITERATE_RECORDS(table_iterator, rec) {
    records.push_back(std::move(rec.value()));
  }
  ASSIGN_OR_RETURN(TupleDesc tuple_desc, catalog->GetTupleDesc(table_id));
  for(int i = 0; i < tuple_desc->Length(); i++){
    ASSIGN_OR_RETURN(Type field_type, tuple_desc->GetFieldType(i));
    if(field_type.GetValue() == Type::Value::INT){
      histograms.push_back(std::make_unique<IntHistogram>(records, i));
    } else {
      histograms.push_back(std::make_unique<StringHistogram>(records, i));
    }
  }


    // calculate histograms for 

  return absl::UnimplementedError("TODO");
}

double TableStats::EstimateScanCost() {
  // TODO
  return 1.0;
}

int TableStats::EstimateTableCardinality() {
  // TODO
  return 1000;
}

double TableStats::AverageSelectivity(int column, execution::Op op) {
  // TODO
  return 1.0;
}

double TableStats::EstimateSelectivity(int column, execution::Op op,
                                       Field* constant) {
  // TODO
  return 1.0;
}

double TableStats::EstimateSelectivity(int lcolumn, execution::Op op,
                                       int rcolumn) {
  // TODO
  return 1.0;
}

};  // namespace komfydb::optimizer
