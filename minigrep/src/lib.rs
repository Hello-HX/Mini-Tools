//引入到库包中，非main的代码

use std::error::Error;
use std::fs;
use std::env;


//聚合配置变量
pub struct Config {
    pub query: String,
    pub file_path: String,
    pub ignore_case: bool,
}

// 命令解析
//这里选择clone, 防止所有权带来的问题
/*
fn parse_config(args: &[String]) -> Config {
   let query = args[1].clone();
   let file_path = args[2].clone();
   Config {query,file_path}
}
*/
//修改为符合 OO 语言的构造

impl Config {
    //由于修改了clone，所以这里要修改
    //而且移除索引
    pub fn build(mut args : impl Iterator<Item = String>) -> Result<Config, &'static str> {
        //错误处理1：主动调用panic
        /*
        if args.len( ) < 3 {
            panic!("not enough arguments");
        }
        */
        //错误处理2：返回Result来直接代替panic
        /* 
        if args.len() < 3 {
            return Err("not enough arguments");
        }
        */

        args.next();

        let query = match args.next() {
            Some(arg) => arg,
            None => return Err("Didn't get a query string"),
        };

        let file_path = match args.next() {
            Some(arg) => arg,
            None => return Err("Didn't get a file path")
        };

        let ignore_case = env::var("IGNORE_CASE").is_ok();
        
        //let query = args[1].clone();
        //let file_path = args[2].clone();

        //klet ignore_case = env::var("IGNORE_CASE").is_ok();

        Ok(Config { query, file_path , ignore_case})
    }
}

//直接所有权转移
//Error是特征对象，引入包，dyn类型，动态，智能指针
pub fn run(config: Config) -> Result<(), Box<dyn Error>> {
    //读取文件内容，记得异常处理
    //使用 ？ 将otherError转换为returnError
    let contents = fs::read_to_string(config.file_path)?;
    //捕获变量
    //print!("With text:\n{contents}");

    let results = if config.ignore_case {
        search_case_insensitive(&config.query, &contents) 
    } else {
        search(&config.query, &contents)
    };

    for line in results {
        println!("{line}");
    }

    Ok(())
}

//大小写敏感
pub fn search<'a >(query: &str,contents: &'a str) -> Vec<&'a str> {
    //vec![] //注定失败的用例
    /* 
    let mut result = Vec::new();
    for line in contents.lines() {
        if line.contains(query) {
            result.push(line);
        }
    }
    result
    */
    contents
        .lines()
        .filter(|line| line.contains(query))
        .collect()
}

//大小写不敏感
pub fn search_case_insensitive<'a >(query: &str,contents: &'a str) -> Vec<&'a str> {
    let query = query.to_lowercase();
    /*
    let mut result = Vec::new();

    for line in contents.lines() {
        if line.to_lowercase().contains(&query) {
            result.push(line);
        }
    }
    result
    */
    contents
        .lines()
        .filter(|line| line.to_lowercase().contains(query.as_str()))
        .collect()
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test] //标注为test
    fn case_sensitive( ) {
        let query = "duct";
        let contents = "\
Rust:
safe, fast, productive.
Pick three.
Duct tape.
";

        assert_eq!(vec!["safe, fast, productive."],search(query,contents));
    }
     
    #[test]
    fn case_insensitive( ) {
        let query = "rUsT";
        let contents = "\
Rust:
safe, fast, productive. 
Pick three. 
Trust me.
";
        assert_eq!(vec!["Rust:", "Trust me."],search_case_insensitive(query, contents));
    }
}


