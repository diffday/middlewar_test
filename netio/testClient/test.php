<?php
require_once("NetUtil.php");

for ($j = 0;$j<5;++$j) {
	$pid = pcntl_fork();
	if ($pid == 0) {
		break;
	}
}

for ($i = 0; $i < 2; ++$i) {
	$cmd = rand(1,2);
	$res = "cmd={$cmd}&pid=" . posix_getpid() . "&i=" . $i;
	$r = NetUtil::tcpCmd("127.0.0.1", 7890, "{$res}",6);
	if (!$r) {
		$res .= " ---this request false, errCode:" . NetUtil::$errCode . " errMsg:" . NetUtil::$errMsg;
		var_dump($res);
	}
	else {
		var_dump($r . "   +++org+++{$res}");
	}
}

