export default {
    data: {
        result: '',
        flag: true,
        todoList: [],
        max:277,
        isVisable:true,
    },
    // 彻底清除
    handleDel() {
        this.result = ''
        this.todoList = []
        if (this.calTextWidth() > this.max) {
            this.isVisable = false
        } else {
            this.isVisable = true
        }
    },
    //删除返回键
    handlCancel() {
        this.flag = true
        this.result = this.result.slice(0, -1)
        this.result = this.addCommas(this.cancalComma(this.result))
        this.todoList = [this.result]
        if (this.calTextWidth() > this.max) {
            this.isVisable = false
            this.$refs.listContain.scrollToEnd()
        } else {
            this.isVisable = true
        }
    },
    calTextWidth() {
        /*
        * number : 11
        * /:8
        * *:9
        * +:11
        * -:10
        * .:4
        * ,:5
         */
        let numberLength = 0;
        let xiaoshuLength = 0;
        let douLength = 0
        let chuLength = 0
        let jianlength = 0
        let chenlength = 0
        let jialength = 0

        for (let i = 0; i < this.result.length; i++) {
            let temp = this.result[i]
            if (temp == '+') {
                jialength++
            } else if (temp == '.') {
                xiaoshuLength++
            } else if (temp == ',') {
                douLength++
            } else if (temp == '-') {
                jianlength++
            } else if (temp == '*'){
                chenlength++
            } else if (temp == '/') {
                chuLength++
            } else {
                numberLength++
            }
        }
        let resultLength = numberLength * 11
            + xiaoshuLength * 4
            + douLength * 5
            + chuLength * 8
            + jianlength * 10
            + chenlength * 9
            + jialength * 11;
        console.log('textWidth  ' + resultLength)
        return resultLength
    },
    calResultLength() {
        let numberLength = 0; //21
        let xiaoshuLength = 0; //9
        let douLength = 0 //10
        let chuLength = 0 //15
        let jianlength = 0 //18

        for (let i = 0; i < this.result.length; i++) {
            let temp = this.result[i]
            if (temp == '+') {
                numberLength++
            } else if (temp == '.') {
                xiaoshuLength++
            } else if (temp == ',') {
                douLength++
            } else if (temp == '-' || temp == '*') {
                jianlength++
            } else if (temp == '/') {
                chuLength++
            } else {
                numberLength++
            }
        }
        let resultLength = numberLength * 12 + xiaoshuLength * 19 + douLength * 10 + chuLength * 15 + jianlength * 18;
        let tempLength = resultLength > 454 ? '': 454;
        console.log('tempLength' + tempLength)
        return tempLength;
    },
    handleClick(data) {
        console.info('data  ' + data)
        if (this.result.length == 0) {
            //首次不能输入 + * /
            if (data == "+" || data == "*" || data == "/") {
                return
            } else {
                this.result = String(data);
                this.todoList = [this.result]
                return
            }
        } else {
            this.result = this.cancalComma(this.result)
            //限制开始不能输入符号
            if (this.result.length == 1) {
                if (this.result[0] == '-' && (data == '*' || data == '+' || data == '/')) {
                    this.result = data
                }
            }

            //异常输出  或 清空上次一次输入
            if (this.result == '错误' || (!this.flag && this.result.length != 0 && typeof (data) == 'number')) {
                this.handleDel()
            }
            this.flag = true;

            //限制不能连续输入符号
            let tempOperator = this.result[this.result.length-1]
            if ((tempOperator == '*' || tempOperator == '/' || tempOperator == '+' || tempOperator == '-') &&
                (data == '*' || data == '/' || data == '+' || data == '-')) {
                this.result = this.result.slice(0, -1)
            }

            let tempResult = [this.result.lastIndexOf("+"), this.result.lastIndexOf("-"), this.result.lastIndexOf("*"),
                this.result.lastIndexOf("/")]
            let resultMax = Math.max(...tempResult)

            //限制连续输入0
            if (typeof (data) == 'number') {
                if (resultMax == -1) {
                    if (this.result[0] == 0 && this.result.indexOf('.') == -1) {
                        this.result = this.result.substring(0)
                    }
                    //补全0
                    if (this.result[0] == '.') {
                        this.result = '0' + this.result
                    }
                } else {
                    //补全0
                    if (this.result.slice(resultMax + 1).length > 0) {
                        //有符号并且符号后面紧跟 小数点
                        if (this.result.slice(resultMax + 1)[0] == '.') {
                            this.result = this.result.slice(0, resultMax) + '0' + this.result.slice(resultMax)
                        }
                        if (this.result.slice(resultMax + 1)[0] == '0' &&
                            this.result.slice(resultMax + 1).indexOf('.') != -1) {
                            this.result = this.result.slice(0, resultMax + 1)
                        }
                    }
                }
                this.result += String(data);
                this.result = this.addCommas(this.result)
                this.todoList = [this.result]

            }
            if (data == '.') {
                if (resultMax == -1 && this.result.indexOf('.') != -1) {
                    return
                }
                if (resultMax != -1 && this.result.slice(resultMax + 1).indexOf('.') != -1) {
                    return
                }
                this.result += String(data);
                this.result = this.addCommas(String(this.result))
                return
            }
            //限制符号的输入
            if (data == '*' || data == '/' || data == '+' || data == '-') {
                if (resultMax == -1) {
                    this.result = String(Number(this.result))
                } else {
                    this.result = this.result.slice(0, resultMax + 1) + String(Number(this.result.slice(resultMax + 1)))
                }
                this.result += String(data);
                this.result = this.addCommas(this.result)
                //return
            }
            //删除

            //结果
            if (data == "=") {
                console.info(" ==== ")
                // 第四种 运算符末尾出现，最好剔除
                if (this.result[this.result.length - 1] == "+" || this.result[this.result.length - 1] == "-" ||
                    this.result[this.result.length - 1] == "*" || this.result[this.result.length - 1] == "/") {
                    this.result = this.result.slice(0, -1)
                }
                if (this.result[0] == "*" || this.result[0] == "/") {
                    this.result = '错误'
                }
                this.result = this.handleData(this.result)
                this.flag = false;
                return
            }
        }
        if (this.calTextWidth() > this.max) {
            this.isVisable = false
            this.$refs.listContain.scrollToEnd()
        } else {
            this.isVisable = true
        }

    },
    handleData(data) {
        let _data = this.cancalComma(data)
        let func = new Function('return ' + _data);   //eval（1+2）   3
        let errorresult = func() == Infinity ? '错误' : this.addCommas(String(func()))
        return errorresult
    },
    //加逗号
    addCommas(str) {
        var result = '';
        if (str.length > 0) {
            let tempResult = [str.lastIndexOf("+"), str.lastIndexOf("-"), str.lastIndexOf("*"), str.lastIndexOf("/")]
            let resultMax = Math.max(...tempResult)
            if (resultMax == -1) {
                result = this.splitStr(str)
            } else {
                let j = 0
                for (var i = 0; i < str.length; i++) {
                    let temStr = ''
                    if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/') {
                        temStr = this.splitStr(str.substring(j, i)) + str.substring(i, i + 1)
                        i++
                        j = i;
                    }
                    result += temStr
                }
                result += this.splitStr(str.substring(j, str.length))
            }
        }
        return result;
    },
    //拆分
    splitStr(str) {
        let result = ''
        if (str.length > 0) {
            if (str.indexOf('e') != -1) {
                return str;
            }
            if (str.indexOf('.') != -1) {
                return this.chartStr(str.substring(0, str.indexOf('.'))) + str.substring(str.indexOf('.'))
            }
            return this.chartStr(str)
        }
        return result;
    },
    chartStr(str) {
        let result = ''
        if (str.length > 0) {
            for (var i = 0; i < str.length; i++) {
                if (i > 0 && i % 3 === 0) {
                    result = ',' + result;
                }
                result = str.charAt(str.length - 1 - i) + result;
            }
        }
        return result;
    },
    //去除逗号
    cancalComma(str) {
        let result = ''
        if (str.length > 0) {
            for (let i = 0; i < str.length; i++) {
                if (str[i] !== ',') {
                    result += str[i];
                }
            }
        }
        return result
    }
}
