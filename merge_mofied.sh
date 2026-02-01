git diff fxs..ssh_send1 --name-only --diff-filter=M | while read file; do
    echo "检查文件: $file"
    git diff fxs..ssh_send1 -- "$file" | head -20
done







