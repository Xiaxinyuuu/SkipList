namespace cpp storage_service

service Storage {
  void insert(1: string key, 2: string val),
  void update_by_key(1: string key, 2: string val),
  void delete_by_key(1: string key),
  string find_by_key(1: string key),
  void dump(1: string file_name),
  void load(1: string file_name)
}

