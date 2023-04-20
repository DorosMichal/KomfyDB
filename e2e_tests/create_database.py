import argparse

from database_creator import DatabaseCreator

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--schema_path", type=str, default="./schema.json")
    parser.add_argument("-o", "--output", type=str, default="./database.sql")
    args = parser.parse_args()
    
    creator = DatabaseCreator(args.schema_path)
    script = creator.get_script()
    
    with open(args.output, 'w') as file:
        file.write(script)

if __name__ == "__main__":
    main()