-- Diagonalize_Test.hs
-- T. M. Kelley
-- Jan 20, 2017
-- (c) Copyright 2017 LANSLLC, all rights reserved

{-# LANGUAGE OverloadedStrings #-}


module Test.Diagonalize_Test (tests) where

import Test.Tasty.QuickCheck (testProperty)
import Test.Tasty (TestTree(..),testGroup,TestName)
import Test.QuickCheck (
  Testable,Arbitrary,arbitrary,choose,Gen,arbitraryBoundedEnum,elements)
import Test.Tasty.HUnit (testCase)
import qualified Test.HUnit as HU

import Diagonalize
import qualified Data.HashMap.Strict as M
import Debug.Trace
import Data.List (sort)

tests :: TestTree
tests = testGroup "Diagonalize tests" [qcgroup,hugroup]
qcgroup = testGroup "Diagonalize properties" qcTests
hugroup = testGroup "Diagonalize cases" huTests

{-
Scenario 1:
Fnc |
    |
n1  | x   x
n0  | x x x
    --------- Fld
-}
c0 =HU.assertBool "reverse funcs_s1 == revFuncs_s1"
  (reverseMap funcs_s1 == revFuncs_s1)
c1 =HU.assertBool "reverse flds_s1 == revFlds_s1"
  (reverseMap flds_s1 == revFlds_s1)
c2 = HU.assertEqual "scorePair scenario 1 pair 1,0 == 1" 1 $
  scorePair data_s1 funcs_s1 revFlds_s1 1 0
c3 = HU.assertEqual "scorePair scenario 1 pair 2,0 == 0" 0 $
  scorePair data_s1 funcs_s1 revFlds_s1 2 0
c4 = HU.assertEqual "scorePair scenario 1 pair 2,1 == 0" 0 $
  scorePair data_s1 funcs_s1 revFlds_s1 2 1
c5 = HU.assertEqual "scoreFldIdx scenario 1 fld 0 == 0" 0 $
  scoreFldIdx data_s1 funcs_s1 revFlds_s1 0
c6 = HU.assertEqual "scoreFldIdx scenario 1 fld 1 == 1" 1 $
  scoreFldIdx data_s1 funcs_s1 revFlds_s1 1
c7 = HU.assertEqual "scoreFldIdx scenario 1 fld 2 == 0" 0 $
  scoreFldIdx data_s1 funcs_s1 revFlds_s1 2
c8 = HU.assertEqual "score scenario 1 == 1" 1 $
  score data_s1 funcs_s1 revFlds_s1
{-
Scenario 2:
Fnc |
n3  |   x
n2  |   x   x
n1  | x   x x
n0  | x x x
    ----------- Fld
-}
c9  = HU.assertEqual "scorePair scenario 2 pair 1,0 == 0" 0 $ scorePair data_s2 funcs_s2 revFlds_s2 1 0
c10 = HU.assertEqual "scorePair scenario 2 pair 2,0 == 0" 0 $ scorePair data_s2 funcs_s2 revFlds_s2 2 0
c11 = HU.assertEqual "scorePair scenario 2 pair 2,1 == 2" 2 $ scorePair data_s2 funcs_s2 revFlds_s2 2 1
c12 = HU.assertEqual "scorePair scenario 2 pair 3,0 == 0" 0 $ scorePair data_s2 funcs_s2 revFlds_s2 3 0
c13 = HU.assertEqual "scorePair scenario 2 pair 3,1 == 1" 1 $ scorePair data_s2 funcs_s2 revFlds_s2 3 1
c14 = HU.assertEqual "scorePair scenario 2 pair 3,2 == 0" 0 $ scorePair data_s2 funcs_s2 revFlds_s2 3 2
c15 = HU.assertEqual "scoreFldIdx scenario 2 fld 0 == 0" 0 $ scoreFldIdx data_s2 funcs_s2 revFlds_s2 0
c16 = HU.assertEqual "scoreFldIdx scenario 2 fld 1 == 0" 0 $ scoreFldIdx data_s2 funcs_s2 revFlds_s2 1
c17 = HU.assertEqual "scoreFldIdx scenario 2 fld 2 == 2" 2 $ scoreFldIdx data_s2 funcs_s2 revFlds_s2 2
c18 = HU.assertEqual "scoreFldIdx scenario 2 fld 3 == 1" 1 $ scoreFldIdx data_s2 funcs_s2 revFlds_s2 3
c19 = HU.assertEqual "score scenario 2 == 3" 3 $ score data_s2 funcs_s2 revFlds_s2
-- consistency checks for scenario 2
c20 = HU.assertEqual "getFlds scenario 2 == flds_s2" flds_s2 (getFlds data_s2)
c21 = HU.assertEqual "getFuncs scenario 2 == funcs_s2" funcs_s2 (getFuncs data_s2)

tc = testCase
tc0  = tc "reverse correct Scenario 1 funcs" c0
tc1  = tc "reverse correct Scenario 1 fields" c1
tc2  = tc "scorePair scenario 1 pair 1,0" c2
tc3  = tc "scorePair scenario 1 pair 2,0" c3
tc4  = tc "scorePair scenario 1 pair 2,1" c4
tc5  = tc "scoreFldIdx scenario 1 fld 0" c5
tc6  = tc "scoreFldIdx scenario 1 fld 1" c6
tc7  = tc "scoreFldIdx scenario 1 fld 2" c7
tc8  = tc "score scenario 1" c8
tc9  = tc "scorePair scenario 2 pair 1,0" c9
tc10 = tc "scorePair scenario 2 pair 2,0" c10
tc11 = tc "scorePair scenario 2 pair 2,1" c11
tc12 = tc "scorePair scenario 2 pair 3,0" c12
tc13 = tc "scorePair scenario 2 pair 3,1" c13
tc14 = tc "scorePair scenario 2 pair 3,2" c14
tc15 = tc "scoreFldIdx scenario 2 fld 0" c15
tc16 = tc "scoreFldIdx scenario 2 fld 1" c16
tc17 = tc "scoreFldIdx scenario 2 fld 2" c17
tc18 = tc "scoreFldIdx scenario 2 fld 3" c18
tc19 = tc "score scenario 2" c19
tc20 = tc "scenario 2 fields consistent" c20
tc21 = tc "scenario 2 funcs consistent" c21

huTests = [tc0,tc1,tc2,tc3,tc4,tc5,tc6,tc7,tc8
  ,tc9,tc10,tc11,tc12,tc13,tc14,tc15,tc16,tc17,tc18,tc19,tc20,tc21
  ]

-- reversing the reverse puts you back where you started
prop_RevFncMap2x_Id :: FncMap -> Bool
prop_RevFncMap2x_Id m = m'' == m
  where m'' = reverseMap (reverseMap m)
-- This is redundant only as long as Fld == Fnc == type alias for Text
prop_RevFldMap2x_Id :: FldMap -> Bool
prop_RevFldMap2x_Id m = m'' == m
  where m'' = reverseMap (reverseMap m)
-- the pairwise score for any field with itself is always 0
prop_ScorePairSelf0 :: DataFncsFldsIdx -> Bool
prop_ScorePairSelf0 (DataFncsFldsIdx d nm lm i) =
  0 == (scorePair d nm (reverseMap lm) i i)
-- Swapping the same field in the field map is the same as leaving the map alone
prop_swapSelf_id :: FldMapIdx -> Bool
prop_swapSelf_id (FldMapIdx m i) = m == swap i i m (reverseMap m)
-- swapping only affects the swapped values and keys
prop_swapOnlyTouchesTwo :: FldMapIdxIdx -> Bool
prop_swapOnlyTouchesTwo (FldMapIdxIdx m i1 i2) = M.foldlWithKey' go True m
  where m' = swap i1 i2 m (reverseMap m)
        go :: Bool -> Fld -> Int -> Bool
        go b f i =
          b && if i /= i1 && i/= i2
            then (m M.! f) == (m' M.! f)
            else True
-- swapping leaves size unchanged
prop_swapKeepsSize :: FldMapIdxIdx -> Bool
prop_swapKeepsSize (FldMapIdxIdx m i1 i2) = M.size m == M.size m'
  where m' = swap i1 i2 m (reverseMap m)

-- swapping correcly exchanges the values and keys
prop_swapCorrectlySwapsTwo :: FldMapIdxIdx -> Bool
prop_swapCorrectlySwapsTwo (FldMapIdxIdx m i1 i2) = M.foldlWithKey' go True m
  where m' = swap i1 i2 m (reverseMap m)
        go :: Bool -> Fld -> Int -> Bool
        go b f i =
          b && if i /= i1 && i/= i2
            then True -- checked in prop_swapOnlyTouchesTwo
            else if i == i1
              then (m' M.! f) == i2
              else (m' M.! f) == i1
-- The maps created by get<Fld|Func>s cover 0 to N - 1
prop_fldValsCover_0_Nm1 :: Data -> Bool
prop_fldValsCover_0_Nm1 d = [0..M.size m - 1] == (sort $ M.elems m)
  where m = getFlds d
prop_funcValsCover_0_Nm1 :: Data -> Bool
prop_funcValsCover_0_Nm1 d = [0..M.size m - 1] == (sort $ M.elems m)
  where m = getFuncs d

-- repositionAfter doesn't change the size of the map
prop_repoUnchangedSize :: FldMapFldFld -> Bool
prop_repoUnchangedSize (FldMapFldFld m f1 f2) = M.size m == M.size m'
  where m' = repositionAfter m f1 f2
-- after calling repositionAfter, the map still covers [0..N-1]
prop_repoAfterStillCovers :: FldMapFldFld -> Bool
prop_repoAfterStillCovers (FldMapFldFld m f g) =
  [0..M.size m - 1] == (sort $ M.elems m')
  where m' = repositionAfter m f g
-- f actually gets repositioned
prop_repoAfterChangesF :: FldMapFldFld -> Bool
prop_repoAfterChangesF (FldMapFldFld m f g) = m' M.! f == 1 + m' M.! g
  where m' = repositionAfter m f g
-- g left alone
prop_repoAfterGUnchanged :: FldMapFldFld -> Bool
prop_repoAfterGUnchanged (FldMapFldFld m f g) = m' M.! g == m M.! g
  where m' = repositionAfter m f g
-- repoAfter doesn't change anything outside of m(g) to m(f)
prop_repoAfterCorrectChanges :: FldMapFldFld -> Bool
prop_repoAfterCorrectChanges (FldMapFldFld m f g) = M.foldlWithKey' go True m'
  where m' = repositionAfter m f g
        i_f = m M.! f
        i_g = m M.! g
        go :: Bool -> Fld -> Int -> Bool
        go b k i = b &&
          if i <= i_g || i > i_f -- outside the changed part of the map
          then m' M.! k == m M.! k
          else if i == i_g + 1  -- f now has the value just past g
            then k == f
            else m' M.! k == 1+m M.! k  -- everyone else moved up 1

tp :: Testable a => TestName -> a -> TestTree
tp = testProperty
tp0  = tp "Reverse (Reverse FncMap) == id" prop_RevFncMap2x_Id
tp1  = tp "Reverse (Reverse FldMap) == id" prop_RevFldMap2x_Id
tp2  = tp "scorePair _ _ _ i i == 0" prop_ScorePairSelf0
tp3  = tp "swap i i _ _ = id" prop_swapSelf_id
tp4  = tp "swap only affects the swapped values" prop_swapOnlyTouchesTwo
tp5  = tp "swap correctly swapped values" prop_swapCorrectlySwapsTwo
tp6  = tp "fldmap via getFlds always covers [0..N-1]" prop_fldValsCover_0_Nm1
tp7  = tp "fncmap via getFuncs always covers [0..N-1]" prop_funcValsCover_0_Nm1
tp8  = tp "swap doesn't change size" prop_swapKeepsSize
tp9  = tp "repoAfter doesn't change size" prop_repoUnchangedSize
tp10 = tp "repoAfter still covers [0..N-1]" prop_repoAfterStillCovers
tp11 = tp "repoAfter really moves f" prop_repoAfterChangesF
tp12 = tp "repoAfter g unchanged" prop_repoAfterGUnchanged
tp13 = tp "repoAfter unchanged outside of (m(g),m(f)]" prop_repoAfterCorrectChanges

qcTests = [tp0,tp1,tp2,tp3,tp4,tp5,tp6,tp7,tp8,tp9,tp10
  ,tp11
  ,tp12
  ,tp13
  ]

data_s1 = [
  ("fnc_0","fld_0"),("fnc_0","fld_1"),("fnc_0","fld_2"),
  ("fnc_1","fld_0"),("fnc_1","fld_2")
  ] :: Data
funcs_s1 = M.fromList [("fnc_0",0),("fnc_1",1)] :: FncMap
flds_s1 = M.fromList [("fld_0",0),("fld_1",1),("fld_2",2)] :: FldMap
revFlds_s1 = M.fromList [(0,"fld_0"),(1,"fld_1"),(2,"fld_2")] :: RevFldMap
revFuncs_s1 = M.fromList [(0,"fnc_0"),(1,"fnc_1")] :: RevFncMap

{-
Scenario 2:
Fnc |
n3  |   x
n2  |   x   x
n1  | x   x x
n0  | x x x
    ----------- Fld
-}
data_s2 = [
  ("fnc_0","fld_0"),("fnc_0","fld_1"),("fnc_0","fld_2"),
  ("fnc_1","fld_0"),("fnc_1","fld_2"),("fnc_1","fld_3"),
  ("fnc_2","fld_1"),("fnc_2","fld_3"),
  ("fnc_3","fld_1")
  ] :: Data
funcs_s2 = M.fromList [("fnc_0",0),("fnc_1",1),("fnc_2",2),("fnc_3",3)] :: FncMap
flds_s2 = M.fromList [("fld_0",0),("fld_1",1),("fld_2",2),("fld_3",3)] :: FldMap
revFlds_s2 = M.fromList [(0,"fld_0"),(1,"fld_1"),(2,"fld_2"),(3,"fld_3")] :: RevFldMap
revFuncs_s2 = M.fromList [(0,"fnc_0"),(1,"fnc_1"),(2,"fnc_2"),(3,"fnc_3")] :: RevFncMap


-- End of file
