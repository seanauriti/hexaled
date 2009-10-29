<?php
    # See if we got here from a form submission
    if ($_REQUEST["last_cmd"])
    {
        $f = fopen($SERIAL_PORT,"rw");

    }
?>
<HTML>
<HEAD>
<TITLE>Alpha One Labs Lights</TITLE>
<SCRIPT language="JavaScript">
<!--
    function do_update(x)
    {
        if (x.readyState != 4) return;
        for (var i=0, j=1; i < 6; ++i, j <<= 1)
        {
            document.forms[0].check[i] = ((ret & j) == j);
        }
    }

    function update_state()
    {
        var x = new XMLHttpRequest();
        x.onReadyStateChange = function() { do_update(x); };
    }

    function box_checked(box)
    {
        
    }

    window.setInterval("update_state()", 3000);
// -->
</SCRIPT>
</HEAD>
<BODY>
<FORM>
<input type="hidden" name="last_cmd"/>
<TABLE border="0">
<?php 
    for ($i=0; $i < 6; ++$i)
    {
        print "<TR>\n";
        print "<TD>$i</TD><TD><INPUT type='checkbox' name='check_' onchange='box_checked($i)' /></TD>\n";
        print "</TR>\n";
    }
?>
</TABLE>
</FORM>
</BODY>
</HTML>
