-- test-main.hs
-- T. M. Kelley
-- Jan 20, 2017
-- (c) Copyright 2017 LANSLLC, all rights reserved

import TestSuite (allTests)
import Test.Tasty as TT (defaultMain)

main :: IO ()
main =
  return allTests >>= TT.defaultMain

-- End of file
