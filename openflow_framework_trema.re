= OpenFlow フレームワーク Trema

#@warn(気の利いたリード文をここに)

第 II 部では、OpenFlow を使ってネットワークをプログラミングする方法を紹介します。職場や自宅のような中小規模ネットワークでもすぐに試せる実用的なコードを通じて、「OpenFlow コントローラってどう作るの？」「OpenFlow って具体的に何に使えるの？」というよくある疑問に答えていきます。題材はなるべく実例を取り入れるようにし、また OpenFlow やネットワークの基礎から説明していくようにしました。ネットワークの専門家はもちろん、普通のプログラマにもすんなり理解できる内容となっているはずです。

まずは、この第 II 部で使う OpenFlow プログラミングのためのフレームワーク、@<ruby>{Trema,トレマ} をあらためて紹介します。

== Trema とは

Trema は OpenFlow コントローラを開発するための Ruby 用のプログラミングフレームワークです。Trema は github 上でオープンに開発しており、GPL2 ライセンスのフリーソフトウェアとして公開されています。公開は 2011 年の 4 月と比較的新しいソフトウェアですが、その使いやすさから国内外の大学や企業および研究機関などですでに数多く採用されています。

Trema の情報は次のサイトから入手できます。

 * Trema ホームページ: @<href>{http://trema.github.com/trema/}
 * github のプロジェクトページ: @<href>{http://github.com/trema/}
 * メーリングリスト: @<href>{http://groups.google.com/group/trema-dev}
 * Twitter アカウント: @<href>{http://twitter.com/trema_news}

Trema の特長は Rails や Sinatra などの Web フレームワークに影響を受けた、アジャイルな OpenFlow プログラミングフレームワークであることです。最近のアジャイル開発環境ではもはや当たり前ですが、Trema はコントローラの開発サイクル全体をサポートする機能を随所にちりばめています。このように開発全体の面倒を見るところが Trema の「フレームワーク」たるゆえんで、他の OpenFlow コントローラやライブラリとはまったく違うところです。第 II 部では、実際に Trema フレームワークを使って OpenFlow コントローラを作っていきます。それではまず Trema をセットアップしましょう。

===[column] @<ruby>{友太郎,ゆうたろう}の質問: Trema の由来ってなに？

#@warn(友太郎のアイコン)

Q. 「こんにちは! 僕は最近 OpenFlow に興味を持ったプログラマ，友太郎です。 Trema っておもしろい名前だけど、何か由来はあるんですか？」
     
A. 筆者の一人が大好きな、大阪の「とれまレコード (@<tt>{http://www.fumiyatanaka.com/})」という小さいレコードレーベルの名前から取りました。さてこの "とれま" とはいったい何でしょう。これは日本がバブルの頃、道路の「とまれ」という標示がよく「とれま」と間違えて描かれていたという事実が元になっています。このありえない誤植の原因は、バブル景気時代に急増した外国人労働者が、道路工事現場でよく日本語をまちがえて「とれま」と描いてしまったことにあるそうです。Trema のストーリーって意外と深いんですね。
   
ちなみに、Trema の公式ロゴマークはこんな画像です。これは Twitter の Trema 公式アカウント (@<tt>{@trema_news}) のアイコンとしても使われています。

//indepimage[trema_logo][][scale=0.7]

もちろん、こんなに大胆な道路標識は日本中どこを探してもありません。この本の編集者が試しに画像編集ソフトで作ったところ評判が良かったので、そのまま公式ロゴになりました。

===[/column]

== Trema のセットアップ

Trema は Linux 上で動作し、次のディストリビューションとバージョンでの動作を保証しています。

#@warn(サポートするバージョンは後で最新版に更新)

 * Ubuntu 12.04, 11.10, 11.04, 10.10, 10.04 (i386/amd64, デスクトップ版)
 * Debian GNU/Linux 6.0 (i386/amd64)

なお保証はしていませんが、RedHat などその他の Linux ディストリビューションでも動作するはずです。

Trema の提供する trema コマンドの実行には root 権限が必要です。まずは、@<tt>{sudo} を使って root 権限でコマンドを実行できるかどうか、@<tt>{sudo} の設定ファイルを確認してください。

//cmd{
% sudo visudo
//}

@<tt>{sudo} ができることを確認したら、Trema のインストールや実行に必要ないくつかのソフトウェアをインストールしましょう。

#@warn(Trema のバージョンは後で最新版に更新)

 * Ruby インタプリタ。Trema は Ruby と C で作成されていて、アプリケーションの記述にも Ruby を使います。Trema では Ruby のバージョン 1.8.7 が必要です。1.8.6 以前のバージョンやバージョン 1.9 以降では今のところ動作しません。
 * Ruby のパッケージ管理システム RubyGems (@<tt>{https://rubygems.org/})。本書では、RubyGems のバージョン 1.8.11 を使っています。
 * Trema 本体。本書は、Trema バージョン 0.2.2.1 を使用して執筆しています。
 * Trema のコンパイルに必要な @<tt>{gcc} や @<tt>{make}、およびいくかのライブラリ。

開発マシンに必要なのはこれだけです。それでは、Ubuntu のパッケージ管理システム @<tt>{apt-get} を使って次のようにインストールしてください。もし他のディストリビューションを使いたい場合は、コマンド名とパッケージ名を適宜読み替えください。

#@warn(file への依存をなくす)
//cmd{
% sudo apt-get install gcc make ruby rubygems ruby-dev irb file libpcap-dev \
  libsqlite3-dev
//}

以上で Trema をインストールするための準備が整いました。Trema をインストールするには、RubyGems のパッケージで手軽にインストールする方法と、最新のソースコードを取得して自分でビルドする方法があります。それぞれ説明していきましょう。

=== パッケージで手軽にインストールする場合

Trema は RubyGems を使って次のようにコマンド一発で簡単にインストールできます。

//cmd{
% gem install trema
//}

RubyGems でインストールした場合、自動的に Trema のコマンド @<tt>{trema} にパスが通っているはずです。次のコマンドでバージョンが表示されればインストールは成功です。

//cmd{
% trema --version
trema version 0.2.2.1
//}

=== ソースコードから最新版をインストールする場合

最新版をインストールしたい人は、github から自分でソースコードをダウンロードしてビルドすることもできます。まず、次のように @<tt>{git} を使って最新のソースコードを取得してください。

//cmd{
% git clone git://github.com/trema/trema.git
//}

次のコマンドを実行すると、さきほどできた @<tt>{trema/} ディレクトリに Trema がインストールされます。@<tt>{make install} のようなシステム全体へのインストールは不要であることに注意してください。

//cmd{
% ./trema/build.rb
//}

次のコマンドで @<tt>{trema/} ディレクトリに @<tt>{trema} コマンドが正しくインストールされたか確認してください。

//cmd{
% ./trema/trema --version
trema version 0.2.2.1
//}

もし必要あればこのディレクトリにパスを通し、@<tt>{trema} コマンドが簡単に起動できるようにしておいてください。

さあ、これで Trema の OpenFlow 開発環境が整いました。それでは早速、入門の定番 Hello, World を Trema で書いてみましょう。

== Hello, Trema!

今から書くアプリケーションは最も簡単な OpenFlow コントーラの一種で、画面に "Hello, Trema!" と表示するだけのものです。まだスイッチと接続して何かしてくれるわけではありませんが、Trema で作れるすべての OpenFlow コントローラのテンプレートとなるアプリケーションです。

では、適当なディレクトリにエディタで @<tt>{hello-trema.rb} というファイルを開き、次のコードを入力してください。"@<tt>{.rb}" は Ruby プログラムの標準的な拡張子です。なお Ruby の文法は必要に応じておいおい説明しますので、今のところは気にせずそのまま入力してください。

//emlist{
  class HelloTrema < Controller
    def start
      puts "Hello, Trema!"
    end
  end
//}

意味はまだわからないかもしれませんが、構成要素も少なくとてもシンプルに見えますね。それでは細かい文法は脇に置いておいて「習うより慣れろ」でさっそく実行してみましょう。

=== 実行してみよう (@<tt>{trema run})

作成したコントローラは @<tt>{trema run} コマンドですぐに実行できます。Ruby はインタプリタ言語なので、コンパイルの必要はありません。ターミナルで次のように入力すると、この世界一短い OpenFlow コントローラはフォアグラウンドプロセスとして起動し、画面に@<tt>{Hello, Trema!} と出力します。起動したコントローラは Ctrl + C で停止できます。

//cmd{
% trema run ./hello-trema.rb
Hello, Trema!  # Ctrl+c で終了
%
//}

いかがでしょうか？ @<tt>{trema} コマンドを使うと、とても簡単にコントローラを書いてすぐに実行できることがわかると思います。@<tt>{trema} コマンドには他にもいくつかの機能がありますのでここで簡単に紹介しておきましょう。

== @<tt>{trema コマンド}

@<tt>{trema} コマンドは Trema 唯一のコマンドラインツールであり、コントローラの起動やテストなど様々な用途に使います。たとえば先ほどの "Hello Trema!" で見たように、@<tt>{trema run} はコントローラを起動するためのコマンドです。起動したコントローラは OpenFlow スイッチと接続しこれを制御します。また、次の章以降で振れますが @<tt>{trema run} コマンドは仮想ネットワークを作ることもでき、コントローラをこの仮想ネットワークの中でテストできます。このように、@<tt>{trema} コマンドは Trema フレームワークにおける中心的なツールで、すべてのコントローラ開発の出発点と言えます。

//indepimage[trema_overview]

@<tt>{trema} コマンドは @<tt>{git} や @<tt>{svn} コマンドと似たコマンド体系を持っており、@<tt>{trema} に続けて @<tt>{run} などのサブコマンドを指定することで様々な機能を呼び出します。こうしたコマンド体系を一般に "コマンドスイート" と呼びます。一般的なコマンドスイートと同じく、サブコマンドの一覧は @<tt>{trema help} で表示できます。また、サブコマンド自体のヘルプは @<tt>{trema help [サブコマンド]} で表示できます。

では @<tt>{trema help} で表示されるサブコマンド一覧をざっと紹介しておきましょう。

: @<tt>{trema run}
  コントローラをフォアグラウンドで実行します。@<tt>{--daemonize (-d)} オプションを付けるとコントローラをバックグラウンド (デーモンモード) として実行できます。

: @<tt>{trema killall}
  バックグラウンドで起動している Trema プロセス全体を停止します。
  
: @<tt>{trema version}
  Trema のバージョンを表示します。@<tt>{trema --version} と同じです。

: @<tt>{trema ruby}
  Trema の Ruby API を表示します。

: @<tt>{trema kill [仮想スイッチまたはリンク]}
  仮想ネットワーク内の指定したスイッチやリンクを停止します。 @<chap>{switch_monitoring_tool} で紹介します。

: @<tt>{trema up [仮想スイッチまたはリンク]}
  仮想ネットワークの指定したスイッチやリンクを再起動します。@<chap>{switch_monitoring_tool} で紹介します。

: @<tt>{trema send_packets [送信オプション]}
  仮想ネットワーク内でテストパケットを送信します。 @<chap>{learning_switch} で紹介します。
  
: @<tt>{trema show_stats [仮想ホスト名]}
  仮想ネットワーク内の仮想ホストで送受信したパケットの統計情報を表示します。@<chap>{learning_switch} で紹介します。

: @<tt>{trema reset_stats}
  仮想ネットワーク内の仮想ホストで送受信したパケットの統計情報をリセットします。@<chap>{learning_switch} で紹介します。

: @<tt>{trema dump_flows [仮想スイッチ名]}
  仮想ネットワーク内の仮想スイッチのフローテーブルを表示します。@<chap>{learning_switch} で紹介。

いくつかのサブコマンドはまだ使い方を紹介していませんが、続く章で説明しますので心配はいりません。ひとまずこの章ではコントローラを起動する @<tt>{trema run} と、Ruby API を表示する @<tt>{trema ruby} コマンドを覚えておいてください。@<tt>{trema ruby} を実行するとデフォルトブラウザで Trema Ruby API のページが開きます。プログラミング中いつでもコマンド一発でリファレンスを開けるので大変便利です。

#@warn(API のページが YARD のせいで壊れているので、直してからスクリーンショット撮り直し)
//indepimage[trema_ruby]

では、気になっていた Ruby の文法にそろそろ進みましょう。第 II 部では今後もたくさん Ruby を使いますが、その都度必要な文法を説明しますので心配はいりません。しっかりついてきてください。

== 即席 Ruby 入門

Ruby を習得する一番の近道は、外国語の習得でもそうですが登場する品詞の種
類を押さえておくことです。Ruby の品詞には、かならず見分けるための手がか
りとなる視覚的なヒントがあります。記号が使われていたり、最初の文字が大
文字になっていたりするので、断片的なコードを見てもすぐにどんな品詞かわ
かります。品詞がわかれば、その Ruby コードがどんな構造かわかります。

それぞれの品詞の見かけに注意してください。それぞれの品詞について簡単に
説明しますが、別に説明が全部理解できなくとも構いません。この節の終わる
頃には "Hello, Trema!" プログラムのあらゆる部分が識別できるようになって
いるはずです。

=== キーワード

Ruby にはたくさんの組み込みの語があり、それぞれに意味が与えられています。
これらの語を変数として使ったり、自分の目的に合わせて意味を変えたりはで
きません。

//quote{
@<tt>{alias   and     BEGIN   begin   break   case    class   def     defined }
@<tt>{do      else    elsif   END     end     ensure  false   for     if }
@<tt>{in      module  next    nil     not     or      redo    rescue  retry }
@<tt>{return  self    super   then    true    undef   unless  until   when }
@<tt>{while   yield}
//}

このうち、"Hello Trema!" の例では @<tt>{class} と @<tt>{def},
@<tt>{end} キーワードを使いました。

//listnum[キーワード][Hello Trema ソースコード内のキーワード]{
@<ami>{class} HelloTrema < Controller
   @<ami>{def} start
     puts "Hello, Trema!"
   @<ami>{end}
@<ami>{end}
//}

@<tt>{class} キーワードは続く名前 (@<tt>{HelloTrema}) のクラスを定義し
ます。このクラス定義は最後の 5 行目の @<tt>{end} までです。@<tt>{def}
キーワードは続く名前 (@<tt>{start}) のメソッドを定義します。このメソッ
ド定義は 4 行目 の @<tt>{end} までです。

=== 定数

定数は Ruby の世界では固有名詞に当たり、必ず大文字で始まります。
@<tt>{Time} や @<tt>{Array} や @<tt>{PORT_NUMBER} などがその例です。

英語でも固有名詞は大文字で始めることになっています。たとえば Tokyo
Tower (東京タワー) もそうです。東京タワーは動かすことができません。東京
タワーを何か別なもののことだと勝手に決めることもできません。固有名詞と
いうのはそういうものです。固有名詞はある特定の、通常は時間とともに変化
しないものを指しています。固有名詞と同様、Ruby の定数は一度セットすると
変更できません。

//quote{
TokyoTower = "東京都港区芝公園 4 丁目 2-8"
//}

"Hello Trema!" の例では @<tt>{class} キーワードに続く
@<tt>{HelloTrema} というクラス名と、@<tt>{Controller} が固有名詞でした。
後で説明しますが、この @<tt>{Controller} は Trema の Ruby ライブラリが
提供するクラス名です。

//listnum[定数][Hello Trema ソースコード内の定数]{
class @<ami>{HelloTrema} < @<ami>{Controller}
  def start
    puts "Hello, Trema!"
  end
end
//}

これで "Hello Trema!" の説明に必要な品詞の説明はおしまいです。それでは
"Hello Trema!" の中身を読み解いていきましょう。

=== コントローラクラスの定義

すべてのコントローラはクラスとして定義され、Trema の @<tt>{Controller}
クラスを継承します (@<list>{コントローラクラス定義}の 1 行目)。
Ruby でクラスを継承するには、@<tt>{class [クラス名] < [親クラス名]} と書きます。

//listnum[コントローラクラス定義][コントローラのクラスを定義する]{
@<ami>{class HelloTrema < Controller}
  def start
    puts "Hello, Trema!"
  end
end
//}

このように @<tt>{Controller} クラスを継承することで、コントローラに必要
な基本機能が @<tt>{HelloTrema} クラスにこっそりと追加されます。雑多な初
期化などの裏仕事を @<tt>{Controller} クラスが代わりにやってくれるわけです。

=== ハンドラメソッドの定義

Trema はイベントドリブンなプログラミングモデルを採用しています。Trema
が定義する各種イベントに対応するハンドラを定義しておくと、イベントが発
生したときに対応するハンドラが自動的に呼び出されます。たとえば
@<tt>{start} メソッド (@<list>{ハンドラ定義}の 2〜4 行目) を定義してお
くと、コントローラの起動時にこれが Trema によって自動的に呼ばれます。

//listnum[ハンドラ定義][起動時に呼ばれるハンドラを定義する]{
class HelloTrema < Controller
  @<ami>{def start}
    @<ami>{puts "Hello, Trema!"}
  @<ami>{end}
end
//}  

@<tt>{start} メソッドの中の @<tt>{puts} は Ruby 組込みのメソッドで、C
の @<tt>{puts()} と同じく指定した文字列を標準出力へ改行付きで出力します。

これで "Hello Trema!" の説明はおしまいです。Trema のコントローラは基本
的に "Hello, Trema!" と同じ構成をしています。つまり、@<tt>{Controller}
を継承するクラスにいくつかハンドラメソッドを追加すればコントローラのできあがりです。

== Trema のファイル構成

最後に Trema のファイル構成を見ておきましょう。Trema をダウンロードする
と、いくつかのファイルとディレクトリがあることがわかります。次に主要な
ものを挙げましょう。

 * @<tt>{build.rb}: ビルドスクリプト。
 * @<tt>{cruise.rb}: すべてのテストコードを実行するテストスイート。Trema 開発者向け。
 * @<tt>{features/}: 受け入れテスト一式。Trema 開発者向け。
 * @<tt>{ruby/}: Ruby ライブラリのソースコード。
 * @<tt>{spec/}: Ruby のユニットテスト一式。Trema 開発者向け。
 * @<tt>{src/examples/}: サンプルアプリ。
 * @<tt>{src/lib/}: C ライブラリのソースコード。
 * @<tt>{tmp}: ログファイルや PID ファイルといった一時ファイルの置き場。
 * @<tt>{trema}: trema コマンド本体。
 * @<tt>{unittests/}: C のユニットテスト一式。Trema 開発者向け。
 
この中でも Trema で何か作りたい人が真っ先に読むべきは、サンプルアプリ
(@<tt>{[trema]/src/examples}) です。

===[column] @<ruby>{取間,とれま}先生曰く: Trema のテスト

Trema にはずいぶんたくさんのテストコードが付いていて、Trema 開発者がテ
ストをとても重視していることがわかるじゃろう。テストの実行頻度も徹底し
ていて、開発者が新しいコードをコミットする度にすべてのテスト
(@<tt>{cruise.rb} スクリプト) を自動的に実行し、「いつダウンロードして
も正しく動く」ことを保証しているのじゃ。これを難しい言葉で "継続的イン
テグレーション" と呼ぶ。

#@warn(テストランプと天井の蛍光灯がかぶって見づらいので、写真を撮り直し)
//image[ccrb][テストの実行結果を示すランプ][scale=0.5]

Trema を壊さないために、ひとつおもしろい工夫があるのじゃ。@<img>{ccrb}
はTrema 開発者の机に置いてあるランプで、テストの実行結果をランプの色で
視覚的にフィードバックする。テストがすべて通るとランプが緑色に光る。も
しエラーが起こった場合には、ランプが赤く光り開発メンバー全員にメールが
飛ぶ。これによって、万が一壊してしまっても必ず誰かが気付けるようにして
いるのじゃ。

このしくみには、環境構築が手軽な CruiseControl.rb
(@<tt>{http://cruisecontrolrb.thoughtworks.com/}) と自作プラグインを使っ
ているぞ。

===[/column]

== サンプルアプリ

サンプルアプリ (@<tt>{[trema]/src/examples/}) は簡単な OpenFlow アプリ
ケーションをたくさん含んでおり、実際の API の使い方を調べるのに便利です。
以下におもなサンプルアプリをまとめます (括弧内は
@<tt>{[trema]/src/examples/} 内のディレクトリ名)。簡単な順に上から並べ
ていますので、この順に読んでいくことをおすすめします。またこのうちいく
つかは続く章で詳しく説明していきます。

: こんにちは Trema (@<tt>{hello_trema})
  この第 II 部で説明する Trema プログラミングのすべての基本となる
  "Hello Trema!" と表示するだけのサンプルです。これを @<tt>{trema run}
  コマンドで実行すれば、手っ取り早く Trema を試すことができます。Trema
  を始めたばかりの初心者向け。

: Packet In (@<tt>{packet_in})
  OpenFlow メッセージの中でも重要な Packet In メッセージをハンドルする
  サンプルです。OpenFlow メッセージハンドラの定義方法や、Packet In メッ
  セージの取り扱いの基本が学べます。

: スイッチの監視 (@<tt>{switch_monitor})
  スイッチがコントローラへ接続したり逆に切断したときのイベントを捕捉す
  るサンプルです。複数のハンドラを使った少し複雑なコントローラの実装が
  学べます。@<chap>{switch_monitoring_tool} で詳しく説明します。

: OpenFlow メッセージのダンプ (@<tt>{dumper})
  コントローラが受け取るすべての OpenFlow メッセージを文字列としてダン
  プするサンプルです。さまざまな OpenFlow メッセージのハンドラの書き方
  のリファレンスとして役に立ちます。

: スイッチ情報 (@<tt>{switch_info})
  スイッチの詳細情報を要求する Features Request メッセージをコントロー
  ラに送信し、スイッチから受信したスイッチ情報を出力するサンプルです。
  コントローラからスイッチへ OpenFlow メッセージを送る方法が学べます。

: リピータハブ (@<tt>{repeater_hub})
  いわゆるバカハブ (ダムハブ) の実装です。重要な OpenFlow メッセージで
  ある Flow Mod と Packet Out の基本が学べます。少し進んだ話題として、
  @<chap>{tdd} ではこれを題材にコントローラのテスト駆動開発手法を学びま
  す。

: ラーニングスイッチ (@<tt>{learning_switch})
  普通のスイッチ (レイヤ 2 スイッチ) をエミュレートするサンプルです。
  FDB などスイッチの基本構成を学ぶことができます。詳しくは
  @<chap>{learning_switch} で説明します。

: トラフィックモニタ (@<tt>{traffic_monitor})
  ラーニングスイッチを拡張し、ユーザごとのトラフィックを測れるようにし
  たものです。フローに含まれる統計情報の利用例として役に立ちます。詳し
  くは @<chap>{traffic_monitor} で説明します。

: 複数スイッチ対応ラーニングスイッチ (@<tt>{multi_learning_switch})
  ラーニングスイッチの複数スイッチ版です。ラーニングスイッチとの違い、
  とくにスイッチごとに FDB を管理する部分に注目してください。

Trema にはたくさんの API があり、この表に掲載したサンプルではまだまだす
べてを紹介しきれていません。新しいサンプルアプリを作った人は、ぜひ
github で pull リクエストを送ってください。あなたの名前が Trema プロジェ
クトの貢献者リスト
(@<tt>{https://github.com/trema/trema/graphs/contributors}) に載るかも
しれません!

== まとめ

さて、これで Trema の基本はおしまいです。この章では Trema をセットアップし、すべてのコントローラのテンプレートとなる Hello, Trema! コントローラを書きました。

 * Trema は RubyGems またはソースコードからビルドしてインストールできます。
 * コントローラは @<tt>{trema run} コマンドでコンパイル無しにすぐ実行できます。
 * コントローラは Ruby のクラスとして定義し、@<tt>{Controller} クラスを継承することで必要なメソッドが取り込まれます。
 * コントローラクラスに各種イベントに対応するハンドラを定義することでロジックを実装できます。たとえば、起動イベントに対応するハンドラは @<tt>{start} です。
 * Trema のファイル構成をおおざっぱに見ました。続く章では Trema のサンプルアプリ (@<tt>{[trema]/src/examples}) のいくつかを題材に、OpenFlow プログラミングを学んでいきます。

次の章では、いよいよ実用的な OpenFlow コントローラを書き実際にスイッチをつないでみます。

== 参考文献

Ruby プログラミングが初めてという人達のために、この章では入門に役立つサイトや本をいくつか紹介します。

: Why's (Poignant) Guide to Ruby (@<tt>{http://mislav.uniqpath.com/poignant-guide/})
  私は大学や職場でいろいろなプログラミング言語を勉強してきましたが、こ
  れほど読んでいて楽しい本に出会ったことはありません。この本は Ruby 界
  の謎の人物 _why 氏による風変りな Ruby 入門本で、まるで小説やマンガの
  ようにリラックスして読めます。この章の Ruby の品詞の説明は、この本を
  参考にしました。日本語版はこちら @<tt>{http://www.aoky.net/articles/why_poignant_guide_to_ruby/}

: TryRuby (@<tt>{http://tryruby.org/})
  同じく _why 氏によるブラウザで動く Ruby 環境です。Ruby を試してみたい
  けどインストールするのが面倒という人は、まずはここで Ruby を試してみ
  ましょう。@<tt>{help} と打つと 15 分の短い Ruby チュートリアルが始ま
  ります。

: プログラミング Ruby (Dave Thomas ら著、オーム社)
  Ruby の完全なリファレンスです。本気で Ruby を勉強したい人は持っていて
  損はしません。この本だけあれば十分です。
