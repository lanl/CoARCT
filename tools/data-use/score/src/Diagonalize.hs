-- Diagonalize.hs
-- T. M. Kelley
-- Jan 19, 2017
-- (c) Copyright 2017 LANSLLC, all rights reserved

{- Try a slightly different approach: the basic data is an unordered
set of pairs (Function,Field). Then we provide mappings of (Fnc|Fld) -> Int.
These mappings are arbitrary. The goal is to find a pair of maps such that
  if i<j,
  then max(funcs(fld(i))) < max(funcs(fld(j)))
With that set of maps, we should be able to easily read off clusters.

To do this, we need

Data :: [(Fnc,Fld)] -- the set of all 'fnc uses fld'

FncMap:: Fnc -> Int  (bijective mapping of functions to ints)
FldMap:: Fld -> Int  (bijective mapping of fields to ints)
RevFncMap:: Int -> Fnc  (bijective mapping of ints to functions)
RevFldMap:: Int -> Fld  (bijective mapping of ints to fields)

reverse :: M.HashMap a Int -> M.HashMap Int a
init :: [a] -> M.HashMap a Int

Then we want to be able to try different numberings:
score :: Data -> FncMap -> FldMap -> Int
swap :: Int -> Int -> M.HashMap a Int -> M.HashMap a Int

To score we need a function
w :: Data -> Fld -> [Fnc]
-}

{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE TupleSections #-}
{-# LANGUAGE DeriveGeneric #-}
{-# LANGUAGE DeriveAnyClass #-}
{-# LANGUAGE GeneralizedNewtypeDeriving #-}
{-# LANGUAGE TypeSynonymInstances #-}
{-# LANGUAGE FlexibleInstances #-}

module Diagonalize where

import qualified Data.ByteString.Lazy as B
import qualified Data.Text as T
import qualified Data.HashMap.Strict as M
import Data.Hashable (Hashable)
import Test.QuickCheck
import Debug.Trace
import Data.List (foldl',nub)
import GHC.Generics (Generic)

{- The Fld type is not really different from Fnc type. If I ever get sick
enough of this then I'll change them. This should be fairly easy. -}
type Map a = M.HashMap a Int
type RevMap a = M.HashMap Int a
type Fnc = T.Text
type Fld = T.Text
type Data = [(Fnc,Fld)]
type FncMap = Map Fnc
type FldMap = Map Fld
type RevFncMap = RevMap Fnc
type RevFldMap = RevMap Fld

-- | Take the function from every pair where the field matches the target.
w :: Data -> Fld -> [Fnc]
w d tfld = fst <$> filter (\(fnc,fld)-> fld == tfld) d

-- | Sum scores for all fields
score :: Data -> FncMap -> RevFldMap -> Int
score d nm rlm = foldl' go 0 [1..(nfld - 1)]
  where nfld = M.size nm
        go i acc = acc + scoreFldIdx d nm rlm i
-- | Score a field index by summing over all field indices less than i
scoreFldIdx :: Data -> FncMap -> RevFldMap -> Int -> Int
scoreFldIdx d nm rlm i = foldl' go 0 [0..(i-1)]
  where go j acc = acc + scorePair d nm rlm i j

{-| Scoring idea: for every pair of fields numbered i,j, with j < i,
  add 1 for each function that sits higher than the maximum field of i.
    vj j = filter (\v -> v > vi i) $ n <$> (w . s $ j) such that vj_l > vi
    vi i = max $ n <$> (w . s $ i)
  where n :: Fnc -> Int
        s :: Int -> Fld
        w :: Fld -> [Fnc]
 -}
scorePair :: Data -> FncMap -> RevFldMap -> Int -> Int -> Int
scorePair d nm rlm i j
  | j <= i =
    let s = (\intg -> rlm M.! intg) :: Int -> Fld
        n = (\fnc -> nm M.! fnc) :: Fnc -> Int
        wd = w d :: Fld -> [Fnc]
        vi i = maximum $ n <$> (wd . s $ i)
        vjs j i = filter (\v -> v > vi i) $ n <$> (wd . s $ j)
    in length $ vjs j i
  | otherwise = error $ "scorePair: j>i (j=" ++ show j ++ ",i="++show i++")"

-- | Create the reverse map of m
-- reverseMap :: Map a -> RevMap a
reverseMap m = M.fromList $ (\(f,i) -> (i,f)) <$> M.toList m

-- | Reposition a field f in the numbering after the indicated field g.
-- This is done by setting m(f) = m(g) + 1 and incrementing every value
-- for all keys k such m(k) >= m(g) + 1 and m(k) < m(f). If m(g) >= m(f)
-- then there is no change; that is, it only moves f lower.
repositionAfter :: FldMap -> Fld -> Fld -> FldMap
repositionAfter m f g
  | (m M.! g >= m M.! f) = m
  | otherwise = M.insert f insV m'
    where oldfv = m M.! f
          insV = 1 + m M.! g
          m' = M.foldlWithKey' go m m
          go :: FldMap -> Fld -> Int -> FldMap
          go fm fld i = if i >= insV && i < oldfv
                        then M.insert fld (i+1) fm
                        else fm

-- | Swap the integer values, i.e.
-- if m ! f = i and
--    m ! g = j
--then m' ! f = j and
--     m' ! g = i
-- Note: rm must == reverse m! Assumption: the reverse map is reused
-- many times.
swap :: (Eq a, Hashable a) => Int -> Int -> Map a -> RevMap a -> Map a
swap i j m rm = let vi = rm M.! i
                    vj = rm M.! j
  in M.insert vj i $ M.insert vi j m

getFuncs :: Data -> FncMap
getFuncs d = M.fromList $ zip funcs [0..length funcs - 1]
  where funcs = nub $ fst <$> d
getFlds :: Data -> FldMap
getFlds d = M.fromList $ zip flds [0..length flds - 1]
  where flds = nub $ snd <$> d

-- related data types and Arbitrary instances for testing

instance Arbitrary Fnc where
  arbitrary = T.pack <$> listOf1 (arbitrary :: Gen Char)
instance Arbitrary FncMap where
  arbitrary = do
    fs <- (listOf arbitrary :: Gen [Fnc]) `suchThat` (\l->length l > 1)
    let is = [0..length fs - 1]
    return $ M.fromList $ zip fs is

-- | Use this to create a consistent FldMap and Index
data FldMapIdx = FldMapIdx {
  dfiFlds :: FldMap,
  dfiIdx :: Int
} deriving (Show,Eq,Read,Generic)

instance Arbitrary FldMapIdx where
  arbitrary = do
    m <- arbitrary
    i <- choose (0,M.size m - 1)
    return $ FldMapIdx m i
-- pick a field map and two unequal indices
data FldMapIdxIdx = FldMapIdxIdx {
  dfiiFlds :: FldMap,
  dfiiIdx1 :: Int,
  dfiiIdx2 :: Int
} deriving (Show,Eq,Read,Generic)
instance Arbitrary FldMapIdxIdx where
  arbitrary = do
    m <- arbitrary
    i1 <- choose (0,M.size m - 1)
    i2 <- choose (0,M.size m - 1) `suchThat`  (\i -> i1 /= i)
    return $ FldMapIdxIdx m i1 i2

-- pick a field map and two unequal fields
data FldMapFldFld = FldMapFldFld {
  fmffFlds :: FldMap,
  fmffF1 :: Fld,
  fmffF2 :: Fld
} deriving (Show,Eq,Read,Generic)
instance Arbitrary FldMapFldFld where
  arbitrary = do
    m <- arbitrary `suchThat` (\m->M.size m > 2)
    let gmax = (M.size m - 2)
    g <- elements (M.keys m) `suchThat` (\g' -> m M.! g' < gmax)
    f <- elements (M.keys m) `suchThat`  (\f' -> (f'/=g) && (m M.! g < m M.! f'))
    return $ FldMapFldFld m f g

-- | Use this to create a consistent Arbitrary Data, FldMap,
-- and FncMap, and field index
data DataFncsFldsIdx = DataFncsFldsIdx {
  dffData :: Data,
  dffFuncs :: FncMap,
  dffFlds :: FldMap,
  dffIdx :: Int
} deriving (Show,Eq,Read,Generic)

instance Arbitrary DataFncsFldsIdx where
  arbitrary = do
    d <- arbitrary :: Gen Data
    let fnmap = getFuncs d
        flmap = getFlds d
    i <- choose (0,M.size flmap-1)
    return $ DataFncsFldsIdx d fnmap flmap i

-- instance Arbitrary Fld where
--   arbitrary = T.pack <$> listOf1 (arbitrary :: Gen Char)
-- instance Arbitrary Data where

-- End of file
