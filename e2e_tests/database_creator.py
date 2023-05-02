from collections import defaultdict
from dataclasses import dataclass
import json
from fields_generators import GenerationType, ValueType, type_map


@dataclass
class ForeignField:
    table_name: str
    field_name: str
    field_type: ValueType
    size: int
    source_table_name: str
    source_field_name: str


class DatabaseCreator:
    """generates sql script describing generated database"""

    def __init__(self, schema_path: str):
        with open(schema_path, "r") as schema_file:
            self.schema = json.load(schema_file)
        self.queries = []

        # table_name -> field name -> value
        self._table_field_values_map = defaultdict(dict)
        # table_name -> field_name -> type
        self._table_field_type_map = defaultdict(dict)

    def set_values(self, table_name, field_name, values):
        self._table_field_values_map[table_name][field_name] = values

    def get_values(self, table_name, field_name):
        return self._table_field_values_map[table_name][field_name]

    def set_type(self, table_name, field_name, value):
        self._table_field_type_map[table_name][field_name] = value

    def get_type(self, table_name, field_name):
        return self._table_field_type_map[table_name][field_name]

    def get_script(self):
        if not self.queries:
            self.create()
        return "\n".join(self.queries)

    def generate_values(self):
        foreign_fields: list[ForeignField] = []
        for table, (size, fields) in self.schema.items():
            for name, field in fields.items():
                field_type = ValueType(field[0])
                generation_type = field[1:]
                values = []
                match generation_type:
                    case ["range", start, stop]:
                        values = type_map[field_type][GenerationType.RANGE](
                            size, start, stop
                        )
                    case ["range", start, stop, number]:
                        values = type_map[field_type][GenerationType.RANGE](
                            size, start, stop, number
                        )
                    case ["unique"]:
                        values = type_map[field_type][GenerationType.UNIQUE](size)
                    case ["foreign", source_table_name, source_field_name]:
                        foreign_fields.append(
                            ForeignField(
                                table,
                                name,
                                field_type,
                                size,
                                source_table_name,
                                source_field_name,
                            )
                        )
                    case [choices] if isinstance(choices, list):
                        values = type_map[field_type][GenerationType.ENUM](
                            size, choices
                        )
                    case _:
                        raise ValueError("cannot match the value")

                self.set_values(table, name, values)
                self.set_type(table, name, field_type)

        for field in foreign_fields:
            primary_keys_values = self.get_values(
                field.source_table_name, field.source_field_name
            )
            values = type_map[field.field_type][GenerationType.FOREIGN](
                field.size, primary_keys_values
            )
            self.set_values(field.table_name, field.field_name, values)

    def create(self):
        self.generate_values()
        # self.begin()
        for table, field_values in self._table_field_values_map.items():
            names = list(field_values.keys())
            values = zip(*field_values.values())

            self.create_table(table, names)
            self.insert_values(table, values)

        # self.commit()

    def begin(self):
        self.queries.append("BEGIN;")

    def commit(self):
        self.queries.append("COMMIT;")

    def create_table(self, table_name, column_names):
        field_descriptions = [
            f"{column_name} {self.get_type(table_name, column_name).value}"
            for column_name in column_names
        ]
        query = f"CREATE TABLE {table_name}({', '.join(field_descriptions)});"
        self.queries.append(query)

    def insert_values(self, table_name, column_values):
        query_start = f"INSERT INTO {table_name} VALUES"
        # our parser understands inserts with single values tuple only
        for values in column_values:
            self.queries.append(f"{query_start} ({', '.join(map(str, values))});")
