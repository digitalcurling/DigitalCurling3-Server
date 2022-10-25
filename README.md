# DigitalCurling3-Server

デジタルカーリング思考エンジンの対戦をホストするサーバーです．
使用方法については[対戦サーバーマニュアル](https://github.com/digitalcurling/DigitalCurling3/wiki/Server)を参照ください．


## リンク

- [DigitalCurling3マニュアル](https://github.com/digitalcurling/DigitalCurling3/wiki)
- [公式サイト](http://minerva.cs.uec.ac.jp/cgi-bin/curling/wiki.cgi)


## ビルド

1. このリポジトリをクローンします．
1. サブモジュールをセットアップするために， `git submodule update --init --recursive` を実行します．
1. [Boost](https://www.boost.org/)をインストールします．
1. [CMake](https://cmake.org/)をインストールします．
1. CMakeがユーザーの `PATH` に入っていることを確認します．
1. 下記のビルドコマンドを実行します．

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

:warning: CMakeがBoostを見つけられない場合は，環境変数`BOOST_ROOT`にBoostをインストールしたディレクトリを設定してください．

## ライセンス

MIT Lisence