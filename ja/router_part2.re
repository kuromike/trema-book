= シンプルなルータ (後編)

//lead{
ルータが持つ重要な機能であるルーティングテーブルの詳細を見ていきましょう。ルータは実に巧妙な仕組みで転送先の情報を管理します。
//}

//indepimage[map][][width=10cm]

== 宛先ホストをまとめる

ルータが管理するルーティングテーブルは、宛先ホストが増えるごとに大きくなります。前編の説明では、ルータは宛先ホスト1つごとにルーティングテーブルのエントリを管理していました。しかしこれでは、たとえばインターネットにホストが加わるごとに、インターネット上のルータはルーティングテーブルを更新する必要があります。しかも、インターネット上のホスト数は直線的に増え続け、2012年現在では10億台を超えると言われています。そうなると、宛先ホストごとにエントリを管理する方法は非現実的です。

これを解決するために、ルータは同じイーサネット上にあるホストをひとつのグループとしてまとめます。そして、ルーティングテーブルの宛先として、ホストではなくこのグループを指定することで、エントリ数を圧縮します。このとき、グループ情報として使うのがネットワークアドレスとネットマスク長です。

//image[router_network][同じイーサネット上にあるホストを一つの宛先にまとめる][width=12cm]

宛先ホストのグループ化は次のように行います。たとえば、@<img>{router_network}の右側のネットワークは、ネットワークアドレスが192.168.1.0でネットマスク長が24です(これを192.168.1.0/24と表現します)。このネットワーク中にあるホストX,Y,Zには、ネットワークアドレスと上位24ビットが同じとなるように、つまりIPアドレスが192.168.1で始まるようにします。こうすれば、ホストX,Y,Zは同じ1つのグループ192.168.1.0/24に属するとみなせます。

このようにアドレスを振ることで、ルータAのルーティングテーブルは、@<img>{router_network}のようにシンプルに書けます。ホストX,Y,Z宛てのパケットを192.168.1.0/24グループ宛てと表現することで、エントリを1つにまとめられるのです。

このとき、ホストX(192.168.1.1)宛のパケットを受け取ったルータAは次のように動作します。ルーティングテーブルのエントリ192.168.1.0/24と、パケットの宛先192.168.1.1との上位24ビットを比較すると一致します。そこで、ルーティングテーブルから次の転送先はルータBだとわかります。ホストY,Z宛も同様に処理できるので、このエントリ1つでホスト3台分の宛先をカバーできます。

=== 宛先ホストがルータと直接つながっているかを調べる

@<img>{router_network} では、ルータが宛先ホストに直接接続していない場合について説明しましたが、つながっている/いないはどのように判断するのでしょうか？

ルータは、その判断のために、自身のインターフェースに割り当てられた IP アドレスを使います。インターフェースに割り当てる IP アドレスには、ネットワーク中のホストとネットワークアドレスが同じ IP アドレスを用います。@<img>{router_address} で、ルータ B のインターフェースには、ホスト X, Y, Z と同じネットワークアドレスになるよう、例えばアドレス 192.168.1.254 を割り当てます。

//image[router_address][ルータのインターフェースには、ネットワーク内のホストとネットワークアドレスが同じとなるよう、IP アドレスを割り当てる][width=12cm]

ここで @<img>{router_address} のルータ B が、ホスト X (192.168.1.1) 宛のパケットを受け取った場合について考えます。ルータ B は、パケットの宛先アドレスを参照し、ネットワークアドレスが同じインターフェースを探します。この例では、192.168.1.254 というアドレスがついたインターフェースが見つかります。あとは、このインターフェースを通じて、ARP リクエストによる MAC アドレス問い合わせを行い、ホスト X 宛にパケットを出力します。

== ネットワーク宛てのエントリをまとめる

複数のホスト宛てエントリをまとめて出来たエントリは、さらにまとめられる場合もあります。

//image[aggregate][複数のネットワークへのルーティング情報をまとめる][width=10cm]

例として、@<img>{aggregate}の3つのネットワークに接続するルータBを考えてみましょう。これら3つのネットワークアドレスは、上位16ビットが172.16.0.0で共通です。ここでルータAから見ると、この3つのネットワークへの次の転送先は、いずれもルータBです。そのため、これら3つのネットワークへのルーティング情報は、172.16.0.0/16宛として1つにまとめられます。

=== 1つの宛先に複数のエントリがマッチする場合

パケットの宛先 IP アドレスに複数のエントリが該当する場合はどうなるでしょうか？@<img>{longest_match} のルータ A がホスト X (172.16.3.1) にパケットを送る場合について考えてみましょう。ルータ A が持つルーティングテーブルは、ルータ B につながる 3 つのネットワーク宛のエントリはまとめることで、@<img>{longest_match}のように 2 つのエントリにできます。しかし、このようにまとめてしまうと、宛先 172.16.3.1 のパケットは、どちらのエントリにもマッチしてしまいます。ルータは、どちらか正しいエントリを選択しなければいけません。

//image[longest_match][マスク長が一番長いエントリを選択する][width=12cm]

複数のエントリにマッチする場合には、ネットマスク長が一番長いエントリを選択するというルールがあります。これをロンゲストマッチと呼びます。ロンゲストマッチにより、ルータAは、ホストX宛のパケットをルータCへと転送し、その結果ホストXへとパケットが届きます。

=== すべての宛先にマッチするデフォルトルート

すべての宛先をまとめたルーティング情報をデフォルトルートと呼び、その宛先を 0.0.0.0/0 と表します。ネットマスク長は、ルーティング情報をまとめるとき、ネットワークアドレスの共通部分の長さを表していました。デフォルトルートでは、まとめられた宛先には共通部分が全くないため、ネットマスク長は 0 となります。

//image[default_route][0.0.0.0/0 は、すべての宛先にマッチする][width=12cm]

@<img>{default_route}のように、インターネットに接続するネットワークでのルーティングテーブルについて考えてみましょう。インターネット上のホスト数は膨大なので、宛先ホストをネットワーク単位にまとめたとしても、数十万エントリを扱う必要があります。しかし、@<img>{default_route}のようにインターネットへの出口が1か所だけの場合、エントリをデフォルトルート1つにまとめられます。これによって、ルーティングテーブル中のエントリ数を大きく減らせます。

@<img>{default_route} のように、インターネットとは別にネットワーク (172.16.3.0/24) があっても、デフォルトルートを使うことに問題はありません。172.16.3.0/24 宛のパケットがルータ A に届いた場合、ルータはロンゲストマッチからルータ C へのエントリを選択します。それ以外のパケットは、デフォルトルートによってルータ B へ転送し、インターネットへと転送します。

== RoutingTable のソースコード

=== パケットを書き換えて転送する(再)

@<tt>{RoutingTable} のソースコードを見る前に、パケットの書き換えと転送を行う @<tt>{forward} メソッドをもう一度見ていきましょう。@<chap>{router_part1} で説明したこのメソッドが行う 5 つの処理のうち、次の転送先と出力インターフェースを決める方法を見ていきます。

//emlist{
  def forward( dpid, message )
    next_hop = resolve_next_hop( message.ipv4_daddr )

    interface = @interfaces.find_by_prefix( next_hop )
    if not interface or interface.port == message.in_port
      return
    end

    arp_entry = @arp_table.lookup( next_hop )
    if arp_entry
      macsa = interface.hwaddr
      macda = arp_entry.hwaddr
      action = create_action_from( macsa, macda, interface.port )
      flow_mod dpid, message, action
      packet_out dpid, message.data, action
    else
      handle_unresolved_packet dpid, message, interface, next_hop
    end
  end
//}

宛先アドレス (@<tt>{message.ipv4_daddr}) に対する次転送先の決定は、@<tt>{resolve_next_hop} メソッドで行なっています。このメソッドは、次のように定義されています。

//emlist{
  def resolve_next_hop( daddr )
    next_hop = @routing_table.lookup( daddr.value )
    if next_hop
      next_hop
    else
      daddr.value
    end
  end
//}

このメソッドでは、まずルーティングテーブル (@<tt>{@routing_table}) を @<tt>{lookup} メソッドで検索し、次の転送先を返します。もし、次の転送先が見つからなかった場合は、宛先アドレス (@<tt>{daddr}) をそのまま返します。これは、@<img>{router_address} で説明したように、宛先ホストとルータが直接接続している可能性があるからです。

その後 @<tt>{forward} メソッドへ戻り、決定した次の転送先がルータのインターフェースに接続しているかを判定します。

//emlist{
    interface = @interfaces.find_by_prefix( next_hop )
    if not interface or interface.port == message.in_port
      return
    end
//}

この判定は、@<tt>{Interfaces} クラスの @<tt>{find_by_prefix} メソッドで行います。このメソッドは、次の転送先と同一のネットワークアドレスであるインターフェースを、ルータが持っているかを調べます。もし、該当するインターフェースがない場合、ルータはそのパケットを転送できないので、パケットを破棄して転送処理を終えます。

=== ルーティングテーブル (@<tt>{RoutingTable}) の実装

次にルーティングテーブルのソースコード(@<list>{routing-table.rb})を見ていきます。

//list[routing-table.rb][ルーティングテーブル (@<tt>{routing-table.rb}) のソースコード]{
require "ipaddr"


class RoutingTable
  ADDR_LEN = 32


  def initialize route = []
    @db = Array.new( ADDR_LEN + 1 ) { Hash.new }
    route.each do | each |
      add each
    end
  end


  def add options
    dest = IPAddr.new( options[ :destination ] )
    masklen = options[ :masklen ]
    prefix = dest.mask( masklen )
    @db[ masklen ][ prefix.to_i ] = IPAddr.new( options[ :nexthop ] )
  end


  def delete options
    dest = IPAddr.new( options[ :destination ] )
    masklen = options[ :masklen ]
    prefix = dest.mask( masklen )
    @db[ masklen ].delete( prefix.to_i )
  end


  def lookup dest
    ( 0..ADDR_LEN ).reverse_each do | masklen |
      prefix = dest.mask( masklen )
      entry = @db[ masklen ][ prefix.to_i ]
      return entry if entry
    end
    nil
  end
end
//}

ルーティングテーブル作成時に呼ばれる @<tt>{initialize} メソッドでは、@<tt>{ADDR_LEN + 1} (= 33) 個のハッシュからなる配列 (@<tt>{@db}) を用意します。ロンゲストマッチを実現するためには、ネットマスク長ごとに分けて経路を管理しなければなりません。そのため、このような実装になっています。

ルーティングテーブルの検索は、@<tt>{lookup} メソッドで行います。このメソッドでは、宛先 @<tt>{dest} に該当するエントリを @<tt>{@db} 中から探します。このとき、ロンゲストマッチが必要なので、@<code>{( 0..ADDR_LEN ).reverse_each} により、ネットマスク長が長い順にハッシュを探索し、見つかった段階でそのエントリを返します。

=== コンフィグ

ルータが動作するためには、インターフェースのアドレスとルーティングテーブルの設定が必要です。シンプルルータでは、これらの設定を @<tt>{simple_router.conf}(@<list>{simple_router.conf})に記述します。

//list[simple_router.conf][シンプルルータのコンフィグ (@<tt>{simple_router.conf})]{
$interface = [
  {
    :port => 2,
    :hwaddr => "00:00:00:01:00:01",
    :ipaddr => "192.168.1.1",
    :masklen => 24
  },
  { 
    :port => 1, 
    :hwaddr => "00:00:00:01:00:02",
    :ipaddr => "192.168.2.1",
    :masklen => 24
  } 
]

$route = [
  {
    :destination => "0.0.0.0", 
    :masklen => 0, 
    :nexthop => "192.168.1.2" 
  }
]
//}

インターフェースの設定では、そのインターフェースの MAC アドレス (@<tt>{:hwaddr})、IP アドレス (@<tt>{:ipaddr})、ネットマスク長 (@<tt>{:masklen}) と、このインターフェースが OpenFlow スイッチのどのポート (@<tt>{:port}) に対応しているかを指定します。

ルーティングテーブルの設定では、宛先 (@<tt>{:destination})、ネットマスク長 (@<tt>{:masklen}) と次の転送先 (@<tt>{:nexthop}) を指定します。

== 実行してみよう

いよいよシンプルルータを動かしてみましょう。いろいろなパケットの送受信を行うために、今回は仮想ホストではなく実際のホストを使用します。シンプルルータとして動作させるサーバも含めて、合計で3台のホストを用意し、@<img>{network}のように接続してください。

//image[network][シンプルルータを動かすための構成][width=10cm]

次に、ホスト1にログインし、以下のようにIPアドレスとデフォルトルートを設定してください。IPアドレスは192.168.1.2/24、デフォルトルートは192.168.1.1です。

//cmd{
host1% sudo ifconfig eth0 192.168.1.2/24
host1% sudo route add -net default gw 192.168.1.1
//}

ホスト2についても同様です。こちらは、IPアドレスを192.168.2.2/24に、デフォルトルートを192.168.2.1に設定します。

//cmd{
host2% sudo ifconfig eth0 192.168.2.2/24
host2% sudo route add -net default gw 192.168.2.1
//}

シンプルルータ用のサーバ上では、仮想ネットワーク機能を使って仮想スイッチを1台起動します。次の設定ファイルを@<tt>{simple_router_network.conf}として用意してください。

//emlist{
vswitch("switch") {
  datapath_id "0x1"
}

link "switch", "eth0"
link "switch", "eth1"
//}

この設定ファイルを指定し@<tt>{trema run}で@<tt>{simple-router.rb}を実行すれば、シンプルルータが起動します。

//cmd{
% trema run ./simple-router.rb -c ./simple_router_network.conf
//}

=== pingで動作を確認する

それでは@<tt>{ping}を使ってシンプルルータが正しく動作しているかを順に確認して行きましょう。まずは、シンプルルータが@<tt>{ping}に応答するかの確認です。ホスト1にログインし、次のようにシンプルルータのIPアドレス192.168.1.1に@<tt>{ping}を打ってみます。

//cmd{
% ping 192.168.1.1
PING 192.168.1.1 (192.168.1.1): 56 data bytes
Request timeout for icmp_seq 0
Request timeout for icmp_seq 1
64 bytes from 192.168.1.1: icmp_seq=2 ttl=64 time=1.806 ms
64 bytes from 192.168.1.1: icmp_seq=3 ttl=64 time=3.629 ms
64 bytes from 192.168.1.1: icmp_seq=4 ttl=64 time=1.475 ms
...
//}

はじめの何回かは@<tt>{ping}の応答が返ってきません。実際のルータでは、ARPが未解決のパケットを受信した時、そのパケットをキューにためておき、ARPが解決した後に転送します。しかし、シンプルルータではこの仕組みを実装していないため、通信の始めではこのように応答が返りません。ただし、いったんARPが解決されてしまえば、後の通信は通常通りに進みます。

次に、シンプルルータをまたいだ二つのホスト間で通信できることも確認してみましょう。ホスト2のIPアドレス192.168.2.2に対して、ホスト1から@<tt>{ping}を送ります。

//cmd{
host1% ping 192.168.2.2
PING 192.168.2.2 (192.168.2.2): 56 data bytes
64 bytes from 192.168.2.2: icmp_seq=0 ttl=64 time=3.715 ms
64 bytes from 192.168.2.2: icmp_seq=1 ttl=64 time=3.271 ms
64 bytes from 192.168.2.2: icmp_seq=2 ttl=64 time=3.367 ms
64 bytes from 192.168.2.2: icmp_seq=3 ttl=64 time=3.545 ms
64 bytes from 192.168.2.2: icmp_seq=4 ttl=64 time=2.153 ms
^C
--- 192.168.2.2 ping statistics ---
5 packets transmitted, 5 packets received, 0.0% packet loss
round-trip min/avg/max/stddev = 2.153/3.210/3.715/0.550 ms
//}

ちゃんと@<tt>{ping}が返ってきました。もちろん、sshやhttpなど今回試した@<tt>{ping}以外の通信も通りますので、いろいろ試してみるとよいでしょう。

== まとめ

ルータ編のまとめとして、最も重要な機能であるルーティングテーブルを詳しく説明しました。

 * ルーティングテーブルの複数のエントリ(宛先がホストのIPアドレス)をひとつのエントリ(宛先がネットワークアドレス)にまとめることで、エントリ数を減らせる
 * こうしてまとめられたエントリは、ネットワークアドレスの一部が同じ他のエントリとまとめることで、さらにエントリ数を減らせる
 * パケットの宛先にマッチするエントリがルーティングテーブルに複数ある場合は、ネットマスクが最も長いエントリを優先 (ロンゲストマッチ) する
 * ルーティングテーブルの宛先に0.0.0.0/0を指定することで、パケットがエントリにマッチしなかった場合のデフォルトの宛先、つまりデフォルトルートを設定できる

次章では、第II部プログラミング編の最終章として、今まで使ってきたTremaの内部構造を解き明かします。
