= パッチパネル

//lead{
執筆中です
//}

スイッチやサーバが増えてくると、ネットワークケーブルの配線は悲惨なまでにごちゃごちゃになります。からみあったケーブルによって見通しが悪くなるのはもちろん、そのままさらに拡張していくと、最悪の場合は配線のやり直しとなります。よくある原因は、スイッチとスイッチ、またはスイッチとサーバ同士を直接つないでしまうという配線方法です。これでは、つなぐ物を増やせば増やすほどごちゃごちゃになるのは当然です。

これを解消するのがパッチパネルという装置です。パッチパネルの仕組みはシンプルで、ケーブルを挿すためのコネクタが並んでいるだけです。これによって配線をいったんパッチパネルで中継できるので、配線がすっきりし拡張性が向上します。また、うまく設計すると配線の変更をパッチパネルだけで行えるようになるので、メンテナンス性も向上します。

(直接つないでごちゃごちゃ → 中継させてすっきり、という図をここに)

パッチパネルをネットワーク経由で操作できるようにしたのがインテリジェントパッチパネルです。パッチパネルでメンテナンス性が向上したとしても、配線を変更するたびにサーバ室まで足を運ぶのは面倒です。そこで、パッチパネルの配線をネットワーク経由で変更できるようにすることで、さらにメンテナンス性が上がります。

実は、このソフトウェアパッチパネルはネットワーク仮想化の一種です。ぶらぶらぶら...

ソフトウェアパッチパネルは OpenFlow を使うと簡単に実装できます。ぶらぶらぶら...

== PatchPanel コントローラ

今回もまずはパッチパネルのソースコード (@<list>{patch-panel.rb}) を眺めることから始めましょう。このソースコードは Trema のサンプルアプリに付属する @<tt>{patch_panel/patch-panel.rb} でも読むことができます。

#@warn(Trema のサンプルに patch_panel/ を追加)
#@warn(SIGHUP か何かで設定ファイルをリロードする仕組みがほしい)

//list[patch-panel.rb][パッチパネル (@<tt>{patch-panel.rb}) のソースコード]{
class PatchPanel < Controller
  def start
    @patch = []
    File.open( "./patch-panel.conf" ).each_line do | each |
      if /^(\d+)\s+(\d+)$/=~ each
        @patch << [ $1.to_i, $2.to_i ]
      end
    end
  end


  def switch_ready datapath_id
    @patch.each do | port_a, port_b |
      make_patch datapath_id, port_a, port_b
    end
  end


  ##############################################################################
  private
  ##############################################################################


  def make_patch datapath_id, port_a, port_b
    send_flow_mod_add(
      datapath_id,
      :match => Match.new( :in_port => port_a ),
      :actions => SendOutPort.new( port_b )
    )
    send_flow_mod_add(
      datapath_id,
      :match => Match.new( :in_port => port_b ),
      :actions => SendOutPort.new( port_a )
    )
  end
end
//}

今までに学んだ知識でこの Ruby ソースコードを解読してみましょう。まず、パッチパネルの本体は @<tt>{PatchPanel} という名前のクラスです。そして@<chap>{switch_monitoring_tool}で学んだ @<tt>{switch_ready} ハンドラの中で @<tt>{make_patch} というプライベートメソッドを呼んでいます。どうやらこれがパッチング処理の本体のようです。

=== 実行してみよう

それでは早速実行してみましょう。今回は仮想ネットワークではなく、実機の OpenFlow スイッチをコントロールしてみます。もし実機を持っていない場合には、@<chap>{diy_switch}を参考に OpenFlow スイッチを自作してみてください。

Trema で作ったコントローラを実機の OpenFlow スイッチと接続するのも、@<tt>{trema run} コマンドだけでできます。仮想ネットワークを使う場合には @<tt>{-c} オプションを @<tt>{trema run} に指定しましたが、実機の場合には @<tt>{-c} オプションなしでコントローラを実行します。

//cmd{
% trema run ./patch-panel.rb
//}

パッチパネルでは、どのポートとどのポートをパッチングするかを設定ファイル (@<tt>{patch-panel.conf}) で指定します。たとえば、ポート 1 番と 10 番、およびポート 2 番と 11 番をパッチングする場合には、次の設定ファイルをコントローラのファイル (@<tt>{patch-panel.rb}) と同じディレクトリに置いてください。

//emlist{
1 10
2 11
//}

== 即席 Ruby 入門

パッチパネルのソースコード (@<list>{patch-panel.rb}) で新しく登場した Ruby の文法を覚えましょう。

=== イテレータ

配列の要素ひとつひとつに対して何か処理をしたいときに使うのがイテレータです。@<tt>{start} ハンドラの中の @<tt>{each_line} や @<tt>{switch_ready} ハンドラの中の @<tt>{each} のように、イテレータはほとんどが @<tt>{each} という名前で始まります。次の例を見れば、イテレータの使い方はすぐ分かるでしょう。

//emlist{
members = [ "友太郎", "青井", "宮坂主任" ]
members.each do | name |
  puts name
end

(出力)
友太郎
青井
宮坂主任
//}

このコードは、3 つの名前が入った配列に対して @<tt>{each} というイテレータを呼び、それぞれの名前を @<tt>{puts} で出力します。@<tt>{do} の右側の @<tt>{| name |} は仮引数で、名前が入ります。そして字下げされたブロックの中の @<tt>{puts} でこの名前を標準出力に出します。

このコードは、次の C コードと意味が同じです。

//emlist{
for ( i = 0; i < 3; i++ ) {
  puts members[ i ];
}
//}

//noindent
C のコードでは、名前の入った配列 @<tt>{members} の要素を順に処理するために、変数 @<tt>{i} を使いました。@<tt>{for} で @<tt>{i = 0, 1, 2} とループを回すことで、@<tt>{members} の要素を順に取り出したのです。Ruby のイテレータは、このループ用変数を使わなくても書けます。仮引数に順に要素がセットされるので、プログラマはループ変数の初期化やインクリメントを気にせずにループでやりたい中身に集中できます。

=== 正規表現

Ruby の正規表現は Perl や他の言語のの正規表現とほとんど変わりません。スラッシュ (@<tt>{/}) ではさまれた文字列が正規表現で、その中に正規表現のメタ文字を書きます。マッチさせるには @<tt>{=~} に続いて文字列を渡します。マッチしなかった場合には @<tt>{nil} が返るので、次のように @<tt>{if} の条件に指定することでマッチしたかどうかを判定できます。

//emlist{
if /taura/=~ "restaurant"
  puts "restaurant は taura を含む"
end

(出力)
restaurant は taura を含む
//}

正規表現で文字列のある部分を取り出したいときには、かっこ @<tt>{()} とマッチ変数 @<tt>{$1}、@<tt>{$2}、… を使います。

//emlist{
if /(\d)\s+Monkeys.+(\d+)\s+Bananas/=~ "12 Monkeys and 24 Bananas"
  puts "猿は #{ $1 } 匹、バナナは #{ $2 } 本"
end

(出力)
猿は 12 匹、バナナは 24 本
//}

もし正規表現がマッチした場合、かっこで囲われた部分にマッチした文字列が先頭から順に @<tt>{$1}、@<tt>{$2}、… にセットされます。

== ソースコード

それでは、パッチパネルのソースコードを詳しく見ていきましょう。

=== 設定ファイル (@<tt>{patch-panel.conf}) の読み込み

@<tt>{start} ハンドラでは、設定ファイル (@<tt>{patch-panel.conf}) を読み込み設定情報をインタンス変数 @<tt>{@patch} に読み込みます。

//emlist{
class PatchPanel < Controller
  def start
    @patch = []
    File.open( "./patch-panel.conf" ).each_line do | each |
      if /^(\d+)\s+(\d+)$/=~ each
        @patch << [ $1.to_i, $2.to_i ]
      end
    end
  end

  # ...
//}

 * @<tt>{@patch} は設定ファイルから読み込んだパッチング情報を入れておく配列です。パッチングするポートのペアを要素に持つ配列の配列となります。たとえばポート 1 番と 10 番、および 2 番と 11 番をパッチングする設定は @<tt>{[[1,10],[2,11]]} となります。
 * @<tt>{File.open(ファイル名)} はファイルを読み込むメソッドで、続く @<tt>{each_line} は読み込んだファイルの一行一行に対してループ処理するためのイテレータです。一時変数 @<tt>{each} にそれぞれの行の内容が文字列として入ります。
 * 正規表現で設定ファイルの各行をパースします。パッチングするポートのペアを @<tt>{$1} と @<tt>{$2} で取り出し、設定情報 @<tt>{@patch} に追加していきます。

=== @<tt>{switch_ready} ハンドラ (パッチング)

@<tt>{switch_ready} ハンドラでは、起動してきたスイッチに対してパッチングの設定を書き込みます。

//emlist{
  # ...

  def switch_ready datapath_id
    @patch.each do | port_a, port_b |
      make_patch datapath_id, port_a, port_b
    end
  end

  # ...
//}

 * @<tt>{@patch.each} は @<tt>{start} ハンドラで読み込んだパッチング情報それぞれを処理するイテレータです。仮引数は @<tt>{port_a} と @<tt>{port_b} の 2 つで、それぞれに配列の第一、第二引数が代入されます。
 * プライベートメソッド @<tt>{make_patch} がパッチング処理の本体です。@<tt>{switch_ready} の引数の Datapath ID、およびパッチングするポート番号 2 つを引数に取ります。

=== @<tt>{make_patch} メソッド (Flow Mod)

@<tt>{make_patch} メソッドではパッチング情報をフローエントリに変換してスイッチに書き込みます。ひとつのパッチングは 2 つのフローエントリになります。つまり、たとえばポート 1 番と 10 番をパッチングしたのであれば、

 * ポート 1 番へ上がってきたパケットをポート 10 番へ出力する。
 * ポート 10 番へ上がってきたパケットをポート 1 番へ出力する。

の 2 つのフローエントリがスイッチに書き込まれます。

//emlist{
  # ...

  def make_patch datapath_id, port_a, port_b
    send_flow_mod_add(
      datapath_id,
      :match => Match.new( :in_port => port_a ),
      :actions => SendOutPort.new( port_b )
    )
    send_flow_mod_add(
      datapath_id,
      :match => Match.new( :in_port => port_b ),
      :actions => SendOutPort.new( port_a )
    )
  end

  # ...
//}

== まとめ
