  files=$(find . -name '*.c' -o -name '*.h') 
  for file in $files; do 
    clang-format -style=file $file | diff $file - >/dev/null 
    if [ $? -ne 0 ]; then \
      echo "::error file=$file::Code not formatted according to clang-format" 
      exit 1 
    fi 
  done
