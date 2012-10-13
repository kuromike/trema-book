= Google のデータセンタ間接続

2012 年 4 月、カリフォルニア州サンタクララで開催された Open Networking Summit において、Google の Urs Hoelzle 氏が衝撃の発表を行いました。Google は早くから OpenFlow に取り組んでおり、Google 内部のネットワークではすでに OpenFlow が動作しているというのです。Internet 界の巨人 Google は、OpenFlow のどのような特徴に着目し、どのように活用しているのでしょうか？Hoelzle 氏の講演内容から、読み解いていきましょう。

== Google が OpenFlow を使う理由

Google が多くの巨大データセンターを保有し、インターネット上の様々な情報を収集していることは、みなさんもご存知でしょう。例えば Google が公開している資料からは、以下のようなデータを見つけることができます。

 * 毎月数十億ページを収集している
 * 毎分 60 時間分の動画が投稿されている
 * ...

これらのデータは、Google では非常に多くのデータを日々扱っていることを示しています。このように大量のデータを処理するためには、Google はスケーラビリティの高いコンピューティング環境を低コストに実現しています。

ネットワークに関してはどうでしょうか？ Google 以外にも、多くの事業者によって、巨大なネットワークの運用が行われています。そもそもインターネットがきちんと動作してることを考えると、今の IP ネットワーク技術は高いスケーラビリティを実現可能であると言ってもよいでしょう。

しかし、Google は既存のネットワーク技術には満足していないようです。特にデータセンター間を繋ぐ WAN 接続に関しては、OpenFlow を用いた新しい試みを行なっています。なぜ彼らは既存の技術に満足しなかったのでしょうか？ポイントは、そのコストにあるようです。

=== 低コスト化への課題

高品質なサービスを如何に低コストに提供するかは、どのサービス事業者にとっても共通の課題でしょう。特に Google のような巨大な設備を持つサービス事業者では、小さなコスト削減の積み重ねが、結果として大きなコスト削減につながります。

しかしながら、ネットワークの低コスト化は容易ではありません。世界最大規模のトラフィックを誇る Google のトラフィックを捌くためには、巨大なネットワーク装置が必要になります。しかし残念ながら、これらの装置は非常に高価であり、大きなコスト要因となっています。

さらに、データセンター間ネットワークの効率的な活用も重要な課題です。Google は世界各国に巨大なデータセンターを多数持っており、これらの間をつなぐ回線も多数有しています。これらの回線は容量が大きなものほど高価になるので、必要最小限の回線を最大限に活用する必要があります。

従来の IP のルーティングでは、宛先に対して最短となるパスが選択され、転送が行われます。回線の使用帯域に関わらず、最短となる経路が選択されるため、トラフィックが集中する回線や、全く使用されない回線が生じる可能性があります。

=== 独自ハードウェアによるコスト削減

 * 10Gbps Ethernet のインターフェイスを 100 ポート
 * OpenFlow サポート
 * BGP や ISIS といった既存のルーティングプロトコルを OSS を使って実装

== WAN 回線の効率的な利用

//image[wan][データセンター B からデータセンター A へどのくらいトラフィックを流せる？]

データセンター間のトラフィックをさばきつつ、回線コストを抑えるためには、WAN 回線の有効活用が必要です。例えば、@<img>{wan} のデータセンター B からデータセンター A へどの程度のトラフィックを流せるか考えてみます。通常の IP ルーティングでは、それぞれのデータセンターに接続するスイッチ間のリンクしか使用しないため、そのリンクの帯域が上限となります。リンク帯域の上限を超えるトラフィックを扱うためには、リンク帯域の増設が必要になり、コストが増える要因となります。しかし、@<chap>{openflow_usecases} で紹介したように、スイッチ C を経由する経路もあわせて使うことで、さらなるトラフィックを流すことができます(@<img>{multipath})。

//image[multipath][複数経路を使ってトラフィックを転送する]

@<img>{multipath} のように複数経路を使うと、どの程度までトラフィック流すことができるでしょうか？他にトラフィックが流れていなければ、それぞれのリンクの帯域の上限まで流すことができます。例えば、リンク 1 の帯域は 10 Gbps であるため、ここには 10Gbps のトラフィックを流すことができます。また複数のリンクを経由する場合、帯域が最小のリンクに合わせる必要があります。リンク 3 の帯域が 6Gbps であるため、スイッチ C を経由する経路で流せるトラフィック量は 6Gbps までとなります。つまりデータセンター B からデータセンター A へのトラフィックは、最大で 16Gbps まで流すことができます。

しかし、この 16Gbps をすべて利用可能なわけではありません。実際には、他のデータセンター間でもトラフィックが流れているため、それらのトラフィックで使用していない帯域しか使用できません。そのため、それぞれのリンクの空き帯域がどれくらいかを調べる必要があります。データセンター B から流せるトラフィック量を調べる場合、リンク 1 とリンク 3 の空き帯域は、それぞれのリンクに接続しているスイッチ B を調べればわかります。しかし、リンク 2 の空き帯域に関しては、なんらかの方法で調べる必要があります。

すべてのリンクの空き帯域を知っていたとしても、まだ課題があります。空き帯域を、どのトラフィックのために使用するかを調停する必要があります。

さらに 16Gbps のトラフィックを二つのパスに分けて転送を行う必要があります。@<chap>{openflow} で説明したように、OpenFlow スイッチではヘッダの情報を見てどちらのパスにパケットを転送するかを決定することしかできません。ちょうど 10Gbps と 6Gbps となるようトラフィックを分けるためには、サービス毎のトラフィック量を知っている必要があります。

まとめると、以下のようになります。

 * 各リンクの空き帯域を知る
 * 空き帯域を、どのトラフィック転送に使うかを決定する
 * トラフィックを、それぞれの経路毎に適切な量に振り分ける

Google が、これらの課題をどのように解決し、WAN の効率化を実現しているかについて、次の章で説明します。

== Google's Software Defined WAN の中身

それでは、Google の WAN がどのように動作しているか見ていきましょう。Google の WAN は、役割に応じて、以下の二種類のネットワークが存在します。

 * I-Scale : Internet に面するバックボーンネットワーク。Google の利用者向けのトラフィックを運ぶ。
 * G-Scale : データセンター間を繋ぐバックボーンネットワーク。内部向けトラフィックを扱う。

このうち OpenFlow を用いている G-Scale について動作を見ていきます。

G-Scale を模式的に示したのが @<img>{interdc-network} になります。

//image[interdc-network][G-Scale はデータセンター間のトラフィックを運ぶ]

G-Scale が、データセンターから受信したパケットを他のデータセンターに適切に届けるためには、以下の二つを行う必要があります。

 * パケットの宛先アドレスを参照し、どのデータセンター宛なのかを調べる
 * 宛先となるデータセンターまでの、どのパスを使ってパケットを転送するかを決定する
 * 決定したパスにそってパケットを転送する

=== 届け先のデータセンターを決定する

データセンターが G-Scale からパケットを受け取るためには、データセンター内にあるホストの IP アドレスを、G-Scale に教えてあげる必要があります。そうしなければ、G-Scale は受け取ったパケットをどのデータセンターに送り届ければいいかを判断できません。

G-Scale では、この目的のために BGP を用いています。BGP は、異なる組織間で、アドレスに関する情報 (経路情報と呼びます) を交換するためのプロコトルです。インターネットのように、様々な組織から構成されるネットワークを作るためには欠かすことのできないプロトコルです。BGP では、それぞれの組織を AS と呼び、AS 単位での経路情報の交換を行います。

BGP は、通常異なる組織同士の接続に用いられますが、Google ではデータセンター間の接続に用いています。各データセンターと G-Scale それぞれを一つの組織に見立て、それぞれが一つの AS として動作しています。G-Scale は、BGP により各データセンターから経路情報をもらい、その情報を元にしてパケットの届け先を決定しています。

//image[bgp][BGP を用いてデータセンター A 内のアドレスを G-Scale に通知する][scale=0.4]

例えば、データセンター A 内のホストには 192.168.1.0/24 のアドレスを持っていたとします @<img>{bgp}。データセンター A 内の BGP ルータはこのアドレスに対する経路情報を BGP を用いて、G-Scale の BGP 処理部に通知します。経路情報を受け取った BGP 処理部は、やはり BGP を用いて、G-Scale 内の他の BGP 処理部へと経路情報を転送します。このようにすることで、例えば宛先が 192.168.1.1 であるパケットを受け取った時に、そのパケットをデータセンター A へと送り届ければよいということを、G-Scale は知ることができます。

===[column] E-BGP と I-BGP

BGP には External BGP (E-BGP) と Internal BGP (I-BGP) の二種類の接続形態があります。E-BGP は異なる組織間の、I-BGP は組織内の情報交換に、それぞれ用います。@<img>{bgp} では、データセンター A の BGP ルータと G-Scale 内の BGP 処理部との間の接続が、E-BGP になります。また G-Scale 内の BGP 処理部同士は I-BGP で接続されています。E-BGP と I-BGP では、受け取った経路情報を転送する際のルールが異なっています。

===[/column]

=== 転送パスを決定する

届け先のデータセンターが決定したら、次はトラフィックを転送するパスを決める必要があります。G-Scale では、ネットワーク中のリンク帯域を有効活用するために、複数のパスを有効に活用します。

//image[bw][空き帯域]

例えば、@<img>[bw] のネットワークにおいて、データセンター B からデータセンター A へのトラフィックについて考えてみます。スイッチ B とスイッチ A を結ぶリンクを通過するパスが最短パスになります。このリンクの空き帯域が 60Gbps だったとすると、最短パスのみを使う従来方式ではこの帯域を超えたトラフィックを転送することができません。しかし、スイッチ C を経由するパスもあわせて使うことで、更に多くのトラフィックを転送できます。

//image[loadbalancing][TE サーバが決めた比率に従って、複数パスでトラフィックを転送]

G-Scale では、流入するトラフィック量を測定し、全体を調停するトラフィックエンジニアリングサーバ (TE サーバ) を導入しています。TE サーバは、各データセンターからの流入トラフィックに関する情報を集めます。そして、各データセンター間のトラフィックを、どのパスを使って転送すべきかという情報を、OpenFlow コントローラへと送ります。OpenFlow コントローラは、この情報を元にフローを設定します。

=== 拠点の構成

G-Scale は、世界中に散らばる Google のデータセンター間を結ぶ巨大なネットワークですが、各データセンターと接続する OpenFlow スイッチや BGP 処理部は、そのデータセンターと同じ拠点に配置されています。各拠点毎の構成は @<img>{quagga-ofc} のようになっています。

//image[quagga-ofc][各拠点ごとに、OpenFlow コントローラと BGP 処理部が配置されている]

各拠点ごとに配置された OpenFlow コントローラは、それぞれの拠点内の OpenFlow スイッチの制御を行なっているだけです。しかし、BGP や TE サーバからの情報を元にフローを決定することで、G-Scale 全体として WAN の帯域の有効活用を実現しています。

ネットワーク中の全 OpenFlow スイッチを一台のコントローラで制御するルーティングスイッチ (@<chap>{routing_switch}) とは異なり、G-Scale は複数の OpenFlow コントローラが協調してネットワークを制御しています。複数コントローラの協調動作については他にさまざまな方法が考えられるため、OpenFlow の仕様自体には規定されていません。G-Scale は、その協調動作の代表的な一例です。

== まとめ

Google が、WAN 回線の有効活用を行うために、データセンター間トラフィックの転送に OpenFlow をどのように活用しているかを紹介しました。

== 参考文献

: Urs Hoelzle 氏 Open Networking Summit 2012 での講演 ( @<tt>{http://youtu.be/VLHJUfgxEO4} )
  今回の取り上げた Urs Hoelzle 氏の講演内容が、Youtube に投稿されています。

: Google を支える技術 (西田圭介著、技術評論社)
  この章ではネットワーク面でのコスト削減について取り上げましたが、この本ではデータセンター自体の運営コストなどについての分析が行われています。この本を読むことで、Google が如何にして何万台ものコンピュータを使ってサービスを実現しているかを学ぶことができます。

: インターネットルーティングアーキテクチャ - BGPリソースの詳解 (サム・ハラビ、ダニー・マクファーソン著、ソフトバンククリエイティブ)
  この章でも簡単に紹介しましたが、より深く BGP について学びたい人は、この本を読むとよいでしょう。具体的な例が多く紹介されており、BGP の動作をイメージしながら理解することができます。