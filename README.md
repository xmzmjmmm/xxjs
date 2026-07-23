手机版本注册curl -s -X POST https://user.xxfanqiang.com/b/reg -d "email=$(date +%s)_$(head -c 8 /dev/urandom | xxd -p)@qq.com&pass=123456&tg=star&ver=1&token=" | python -m json.tool
