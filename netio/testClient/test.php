<?php
require_once("NetUtil.php");

for ($i = 0; $i < 50; ++$i) {
	$cmd = rand(1,2);
	var_dump(NetUtil::tcpCmd("127.0.0.1", 7890, "cmd={$cmd}&act=2\n",6));
}

