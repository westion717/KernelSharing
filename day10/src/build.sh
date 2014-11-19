#build.sh

echo 开始清理
make clean
echo 开始编译
make
echo OK
echo 生成iso
make iso
echo 完成了

