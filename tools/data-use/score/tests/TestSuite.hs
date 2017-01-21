-- TestSuite.hs
-- T. M. Kelley
-- Jan 20, 2017
-- (c) Copyright 2017 LANSLLC, all rights reserved

module TestSuite (allTests) where

import Test.Tasty (testGroup,TestTree(..))
import Test.Diagonalize_Test as Diagonalize

allTests :: TestTree
allTests = testGroup "all tests" [
             Diagonalize.tests
            ]


-- End of file
