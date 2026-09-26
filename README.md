手机版本注册curl -s -X POST https://user.xxfanqiang.com/b/reg -d "email=$(date +%s)_$(head -c 8 /dev/urandom | xxd -p)@qq.com&pass=123456&tg=star&ver=1&token=" | python -m json.tool
闲来无事，逆向了一下最新版本发现/data/user/0/com.star.channel.rudy/no_backup/phoenix/存了明文配置
