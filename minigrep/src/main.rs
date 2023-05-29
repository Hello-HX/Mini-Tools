//in main.rs

//引入环境包，可以处理传入的参数
use std::env;
use std::process;

use minigrep::Config;

fn main() {
    //将内容转换为数组集合
    //let args: Vec<String> = env::args().collect();

    //待搜查字符串
    //存储文件路径
    //let query = &args[1];
    //let file_path = &args[2];
    //2.0
    //let config = parse_config(&args);
    //更加友好的错误处理

    //这里直接传入迭代器即可
    let config = Config::build(env::args()).unwrap_or_else(|err| {
        eprintln!("Problem parsing arguments: {err}");
        process::exit(1);
    });

    //println!("Searching for {}", config.query);
    //println!("In file {}", config.file_path);
    
    //因为只需要匹配一个，用if let 进行匹配错误，不用match
    if let Err(e) = minigrep::run(config) {
        eprintln!("Application error: {e}");
        process::exit(1);
    }

    //使用 dbg！宏，读取数组内容
    //dbg!(args);
}




