#!/bin/bash

#获取传入第一个参数
workspace=$1
echo ${workspace}

cd $workspace

dpkg-buildpackage -b -d -uc -us

#获取当前脚本所在绝对路径
project_path=$(cd `dirname $0`; pwd)
echo ${project_path}

#获取工程名
project_name="${project_path##*/}"
echo ${project_name}

#获取打包生成文件夹路径
pathname=$(find . -name obj*)
echo $pathname

cd $pathname/tests
mkdir -p coverage
#收集gcov信息到.info文件中
lcov -d ../ -c -o ./coverage/coverage.info
#提取特定数据的覆盖率数据
lcov --extract ./coverage/coverage.info '*/src/*' -o ./coverage/coverage.info
#过滤一些我们不感兴趣的文件的覆盖率信息
lcov --remove ./coverage/coverage.info '*/tests/*' -o ./coverage/coverage.info

mkdir ../report

genhtml -o ../report ./coverage/coverage.info

exit 0