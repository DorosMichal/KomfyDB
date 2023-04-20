DatabaseCreator class allows to generate sql script creating table with values base on
provided json schema. Schema must conform to the following grammar

schema -> tables
table -> [size, fields]
field -> [type, values]
type -> "int" | "str"
values -> "range", start, stop, <number>        | // optional number - how many different values
          [val1, val2, ...]                     | // choose randomly from given set
          "unique"                              | // make element unique in every row
          "foreign", "table_name", "field_name"   // make a foreing key
